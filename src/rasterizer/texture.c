/*
 * texture.c
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "texture.h"
#include "ext_math.h"
#include "error.h"
#include "helper.h"
#include "image.h"

#include <math.h>
#include <stdlib.h>


// --- internals --- //

static void _texture_subimage2d(
    PRubyte* texels, PRubyte mip, PRtexsize width, PRtexsize height, PRenum format, const PRvoid* data, PRboolean dither)
{
    if (format != PR_IMAGE_FORMAT_UBYTE_RGB)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return;
    }

    // Setup structure for sub-image
    pr_image subimage;
    subimage.width      = width;
    subimage.height     = height;
    subimage.format     = 3;
    subimage.defFree    = PR_TRUE;
    subimage.colors     = (PRubyte*)data;

    _pr_image_color_to_colorindex_r3g3b2(texels, &subimage, dither);
}

static void _texture_subimage2d_rect(
    pr_texture* texture, PRubyte mip, PRtexsize x, PRtexsize y, PRtexsize width, PRtexsize height, PRenum format, const PRvoid* data)
{
    //...
}

static PRubyte _color_box4_blur(PRubyte a, PRubyte b, PRubyte c, PRubyte d)
{
    PRint x;
    x  = a;
    x += b;
    x += c;
    x += d;
    return (PRubyte)(x / 4);
}

static PRubyte _color_box2_blur(PRubyte a, PRubyte b)
{
    PRint x;
    x  = a;
    x += b;
    return (PRubyte)(x / 2);
}

static PRubyte* _image_scale_down_ubyte_rgb(PRtexsize width, PRtexsize height, const PRubyte* data)
{
    #define COLOR(x, y, i) data[((y)*width + (x))*3 + (i)]

    const PRtexsize scaledWidth = (width > 1 ? width/2 : 1);
    const PRtexsize scaledHeight = (height > 1 ? height/2 : 1);

    PRubyte* scaled = PR_CALLOC(PRubyte, scaledWidth*scaledHeight*3);

    if (width > 1 && height > 1)
    {
        for (PRtexsize y = 0; y < scaledHeight; ++y)
        {
            for (PRtexsize x = 0; x < scaledWidth; ++x)
            {
                for (PRint i = 0; i < 3; ++i)
                {
                    scaled[(y*scaledWidth + x)*3 + i] = _color_box4_blur(
                        COLOR(x*2    , y*2    , i),
                        COLOR(x*2 + 1, y*2    , i),
                        COLOR(x*2 + 1, y*2 + 1, i),
                        COLOR(x*2    , y*2 + 1, i)
                    );
                }
            }
        }
    }
    else if (width > 1)
    {
        for (PRtexsize x = 0; x < scaledWidth; ++x)
        {
            for (PRint i = 0; i < 3; ++i)
            {
                scaled[x*3 + i] = _color_box2_blur(
                    COLOR(x*2    , 0, i),
                    COLOR(x*2 + 1, 0, i)
                );
            }
        }
    }
    else if (height > 1)
    {
        for (PRtexsize y = 0; y < scaledHeight; ++y)
        {
            for (PRint i = 0; i < 3; ++i)
            {
                scaled[y*scaledWidth*3 + i] = _color_box2_blur(
                    COLOR(0, y*2    , i),
                    COLOR(0, y*2 + 1, i)
                );
            }
        }
    }

    return scaled;

    #undef COLOR
}

static PRubyte* _image_scale_down(PRtexsize width, PRtexsize height, PRenum format, const PRvoid* data)
{
    switch (format)
    {
        case PR_IMAGE_FORMAT_UBYTE_RGB:
            return _image_scale_down_ubyte_rgb(width, height, (const PRubyte*)data);
        default:
            break;
    }
    return NULL;
}

// --- interface --- //

pr_texture* _pr_texture_create()
{
    // Create texture
    pr_texture* texture = PR_MALLOC(pr_texture);

    texture->width  = 0;
    texture->height = 0;
    texture->mips   = 0;
    texture->texels = NULL;

    return texture;
}

void _pr_texture_delete(pr_texture* texture)
{
    if (texture != NULL)
    {
        PR_FREE(texture->texels);
        PR_FREE(texture);
    }
}

PRboolean _pr_texture_image2d(
    pr_texture* texture, PRtexsize width, PRtexsize height, PRenum format, const PRvoid* data, PRboolean dither, PRboolean generateMips)
{
    // Validate parameters
    if (texture == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER, __FUNCTION__);
        return PR_FALSE;
    }
    if (width == 0 || height == 0)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return PR_FALSE;
    }

    // Determine number of texels
    PRubyte mips = 0;
    size_t numTexels = 0;

    if (generateMips != PR_FALSE)
    {
        PRtexsize w = width;
        PRtexsize h = height;

        while (1)
        {
            // Count number of texels
            numTexels += w*h;
            ++mips;

            if (w == 1 && h == 1)
                break;

            // Halve MIP size
            if (w > 1)
                w /= 2;
            if (h > 1)
                h /= 2;
        }
    }
    else
    {
        mips = 1;
        numTexels = width*height;
    }

    // Check if texels must be reallocated
    if (texture->width != width || texture->height != height || texture->mips != mips)
    {
        // Setup new texture dimension
        texture->width  = width;
        texture->height = height;
        texture->mips   = mips;

        // Free previous texels
        PR_FREE(texture->texels);

        // Create texels
        texture->texels = PR_CALLOC(PRubyte, numTexels);
    }

    // Fill image data of first MIP level
    PRubyte* texels = texture->texels;

    _texture_subimage2d(texels, 0, width, height, format, data, dither);

    if (generateMips != PR_FALSE)
    {
        PRvoid* prevData = (PRvoid*)data;

        // Fill image data
        for (PRubyte mip = 1; mip < texture->mips; ++mip)
        {
            // Goto next texel MIP level
            texels += width*height;

            // Scale down image data
            data = _image_scale_down(width, height, format, prevData);

            if (mip > 1)
                PR_FREE(prevData);
            prevData = (PRvoid*)data;

            if (data == NULL)
            {
                _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
                return PR_FALSE;
            }

            // Halve MIP size
            if (width > 1)
                width /= 2;
            if (height > 1)
                height /= 2;

            // Fill image data for current MIP level
            _texture_subimage2d(texels, mip, width, height, format, data, dither);
        }

        PR_FREE(prevData);
    }

    return PR_TRUE;
}

PRboolean _pr_texture_subimage2d(
    pr_texture* texture, PRubyte mip, PRtexsize x, PRtexsize y, PRtexsize width, PRtexsize height, PRenum format, const PRubyte* data, PRboolean dither)
{
    // Validate parameters
    if (texture == NULL)
    {
        _pr_error_set(PR_ERROR_NULL_POINTER, __FUNCTION__);
        return PR_FALSE;
    }
    if (texture->texels == NULL || x < 0 || y < 0 || x + width >= texture->width || y + height >= texture->height || width <= 0 || height <= 0 || mip >= texture->mips)
    {
        _pr_error_set(PR_ERROR_INVALID_ARGUMENT, __FUNCTION__);
        return PR_FALSE;
    }

    // Fill image data for specified MIP level
    _texture_subimage2d_rect(texture, mip, x, y, width, height, format, data);

    return PR_TRUE;
}

PRubyte _pr_texture_num_mips(PRubyte maxSize)
{
    return maxSize > 0 ? (PRubyte)(ceilf(log2f(maxSize))) + 1 : 0;
}

/*
TODO: This should be optimized:
add "PRubyte** mipTexels" member to "pr_texture" structure with pointer offsets.
*/
PRubyte* _pr_texture_select_miplevel(pr_texture* texture, PRubyte mip, PRtexsize* width, PRtexsize* height)
{
    mip = PR_CLAMP(mip, 0, texture->mips - 1);

    PRtexsize w = texture->width;
    PRtexsize h = texture->height;

    // Store mip size in output parameters
    *width = PR_MIP_SIZE(w, mip);
    *height = PR_MIP_SIZE(h, mip);

    // Select MIP level texels
    PRubyte* texels = texture->texels;

    while (mip > 0)
    {
        texels += w*h;

        if (w > 1)
            w /= 2;
        if (h > 1)
            h /= 2;

        --mip;
    }

    return texels;
}

PRubyte _pr_texutre_compute_miplevel(pr_texture* texture, PRfloat pixelArea, PRfloat texelArea)
{
    texelArea *= (PRfloat)(texture->width*texture->height) * 0.5f;
    return (PRubyte)PR_CLAMP(texelArea / pixelArea, 0, texture->mips - 1);
}

PRubyte _pr_texture_sample_nearest(PRubyte* mipTexels, PRtexsize width, PRtexsize height, PRfloat u, PRfloat v)
{
    // Clamp texture coordinates
    PRint x = (PRint)((u - (PRint)u)*width);
    PRint y = (PRint)((v - (PRint)v)*height);

    if (x < 0)
        x += width;
    if (y < 0)
        y += height;

    // Sample from texels
    return mipTexels[y*width + x];
}
