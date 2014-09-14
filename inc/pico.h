/*
 * pico.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_PICO_H__
#define __PR_PICO_H__


#include "types.h"
#include "error_ids.h"
#include "platform.h"
#include "enums.h"
#include "structs.h"
#include "consts.h"

#include <stdio.h>


// --- common --- //

//! Initializes the pico renderer.
PRboolean prInit();

//! Releases the pico renderer.
PRboolean prRelease();

//! Returns the last error. By default PR_ERROR_NONE.
PRenum prGetError();

//! Sets the error event handler.
void prErrorHandler(PR_ERROR_HANDLER_PROC errorHandler);

/**
Returns the specified string or null if 'str' is invalid.
\param[in] str Specifies the string which is to be returned. Must begin with 'PR_STRING_...'.
*/
const char* prGetString(PRenum str);

// --- context --- //

/**
Generates a new render context. At least one render context is required to render anything.
\param[in] desc Specifies the OS dependent render context description.
\param[in] width Specifies the context width.
\param[in] height Specifies the context height.
\remarks The render context must be deleted with 'prDeleteContext'.
\see prDeleteContext
*/
PRobject prGenContext(const pr_context_desc* desc, PRuint width, PRuint height);

/**
Deletes the specified render context.
\param[in] context Specifies the render context which is to be deleted.
This must be generated by 'prGenContext'.
\see prGenContext
*/
void prDeleteContext(PRobject context);

//! Presents the currently bound frame buffer in the specified render context.
void prPresent(PRobject context);

// --- framebuffer --- //

/**
Generates a new frame buffer. At least one frame buffer is required to render anything.
\param[in] width Specifies the frame buffer width.
\param[in] height Specifies the frame buffer height.
\return Frame buffer object.
\remarks The frame buffer must be deleted with 'prDeleteFrameBuffer'.
\see prDeleteFrameBuffer
*/
PRobject prGenFrameBuffer(PRuint width, PRuint height);

/**
Deletes the specified frame buffer.
\param[in] frameBuffer Specifies the frame buffer which is to be deleted.
This must be generated by 'prGenFrameBuffer'.
\see prGenFrameBuffer
*/
void prDeleteFrameBuffer(PRobject frameBuffer);

/**
Binds the specified frame buffer.
\param[in] frameBuffer Specifies the frame buffer which is to be bound.
If this is zero, no frame buffer is bound.
*/
void prBindFrameBuffer(PRobject frameBuffer);

/**
Clears the specified frame buffer.
\param[in] frameBuffer Specifies the frame buffer which is to be cleared.
\param[in] clearColor Specifies the color index to clear the frame buffer colors.
\param[in] depth Specifies the depth value to clear the frame buffer depths.
*/
void prClearFrameBuffer(PRubyte clearColor, float depth);

//! Converts the specified RGB color into a color index.
PRubyte prGetColorIndex(PRubyte red, PRubyte green, PRubyte blue);

// --- texture --- //

/**
Generates a new texture.
\return Texture object.
\remarks The texture must be deleted with 'prDeleteTexture'.
\see prDeleteTexture
*/
PRobject prGenTexture();

/**
Deletes the specified texture.
\param[in] texture Specifies the texture which is to be deleted.
This must be generated by 'prGenTexture'.
\see prGenTexture
*/
void prDeleteTexture(PRobject texture);

/**
Binds the specified texture.
\param[in] texture Specifies the texture which is to be bound.
If this is zero, no texture is bound.
*/
void prBindTexture(PRobject texture);

/**
Sets the 2D image data to the specified texture.
\param[in] texture Specifies the texture whose image data is to be set.
\param[in] width Specifies the image width. This will be the final texture width.
\param[in] height Specifies the image height. This will be the final texture height.
\param[in] format Specifies the image data format. This must be PR_IMAGE_FORMAT_UBYTE_RGB.
\param[in] data Raw pointer to the image data. This must be in the format: PRubyte[width*height*3].
\param[in] dither Specifies whether dithering is to be applied to the image (to compensate 8-bit colors).
\param[in] generateMips Specifies whether MIP maps are to be generated for this texture.
*/
void prTextureImage2D(
    PRobject texture, PRtexsize width, PRtexsize height, PRenum format,
    const PRvoid* data, PRboolean dither, PRboolean generateMips
);

/**
Sets the 2D image data from file to the specified texture.
\param[in] texture Specifies the texture whose image data is to be set.
\param[in] filename Specifies the image filename. Valid image file formats are: BMP, PNG, TGA, JPEG (base line only).
\param[in] dither Specifies whether dithering is to be applied to the image (to compensate 8-bit colors).
\param[in] generateMips Specifies whether MIP maps are to be generated for this texture.
\see prTextureImage2D
*/
void prTextureImage2DFromFile(PRobject texture, const char* filename, PRboolean dither, PRboolean generateMips);

// --- vertexbuffer --- //

/**
Generates a new vertex buffer with the specified number of vertices.
\return Vertex buffer object.
\remarks The vertex buffer must be deleted with 'prDeleteVertexBuffer'.
\see prDeleteVertexBuffer
*/
PRobject prGenVertexBuffer();

/**
Deletes the specified vertex buffer.
\param[in] vertexBuffer Specifies the vertex buffer which is to be deleted.
This must be generated by 'prGenVertexBuffer'.
\see prGenVertexBuffer
*/
void prDeleteVertexBuffer(PRobject vertexBuffer);

/**
Sets the vertex buffer data.
\param[in] vertexBuffer Specifies the vertex buffer whose vertex data is to be set.
\param[in] vertices Pointer to the vertex data.
\param[in] numVertices Specifies the number of vertices. The array 'vertices' must be large enough!
\see PRvertex
*/
void prVertexBufferData(PRobject vertexBuffer, const PRvertex* vertices, PRsizei numVertices);

/**
Reads and sets the vertex buffer data from the specified file.
\param[in] vertexBuffer Specifies the vertex buffer whose vertex data is to be set.
\param[out] numVertices Specifies the resulting number of vertices.
\param[in] file Pointer to the file object. This file must be opened in binary read mode: fopen(filename, "rb").
\code
// File format:
numVertices: 16-bit unsigned integer
vertices[numVertices]: 'numVertices' * (five 32-bit floating point values for: x, y, z, u, v) (see 'PRvertex').
\endcode
\see PRvertex
*/
void prVertexBufferDataFromFile(PRobject vertexBuffer, PRushort* numVertices, FILE* file);

/**
Binds the specified vertex buffer.
\param[in] vertexBuffer Specifies the vertex buffer which is to be bound.
If this is zero, no vertex buffer is bound.
*/
void prBindVertexBuffer(PRobject vertexBuffer);

// --- indexbuffer --- //

/**
Generates a new index buffer with the specified number of indices.
\return Index buffer object.
\remarks The index buffer must be deleted with 'prDeleteIndexBuffer'.
\see prDeleteIndexBuffer
*/
PRobject prGenIndexBuffer();

/**
Deletes the specified index buffer.
\param[in] indexBuffer Specifies the index buffer which is to be deleted.
This must be generated by 'prGenIndexBuffer'.
\see prGenIndexBuffer
*/
void prDeleteIndexBuffer(PRobject indexBuffer);

/**
Sets the index buffer data.
\param[in] indexBuffer Specifies the index buffer whose vertex data is to be set.
\param[in] indices Pointer to the index data. Only 16-bit unsigned integers are allowed as vertex indices.
\param[in] numIndices Specifies the number of indices. The array 'indices' must be large enough!
*/
void prIndexBufferData(PRobject indexBuffer, const PRushort* indices, PRsizei numIndices);

/**
Reads and sets the index buffer data from the specified file.
\param[in] indexBuffer Specifies the index buffer whose index data is to be set.
\param[out] numIndices Specifies the resulting number of indices.
\param[in] file Pointer to the file object. This file must be opened in binary read mode: fopen(filename, "rb").
\code
// File format:
numIndices: 16-bit unsigned integer
indices[numVertices]: 'numIndices' * (16-bit unsigned integer).
\endcode
*/
void prIndexBufferDataFromFile(PRobject indexBuffer, PRushort* numIndices, FILE* file);

/**
Binds the specified index buffer.
\param[in] indexBuffer Specifies the index buffer which is to be bound.
If this is zero, no index buffer is bound.
*/
void prBindIndexBuffer(PRobject indexBuffer);

// --- matrices --- //

/**
Sets the projection matrix.
\param[in] matrix4x4 Raw pointer to a 4x4 left-handed projection matrix (in projection space).
\see prBuildPerspectiveProjection
\see prBuildOrthogonalProjection
*/
void prProjectionMatrix(const PRfloat* matrix4x4);

/**
Sets the view matrix.
\param[in] matrix4x4 Raw pointer to a 4x4 left-handed view matrix (in view space).
*/
void prViewMatrix(const PRfloat* matrix4x4);

/**
Sets the world matrix.
\param[in] matrix4x4 Raw pointer to a 4x4 left-handed world matrix (in world space).
*/
void prWorldMatrix(const PRfloat* matrix4x4);

/**
Builds a 4x4 left-handed perspective projection matrix.
\param[out] matrix4x4 Raw pointer to the resulting 4x4 matrix.
\param[in] aspectRatio Specifies the perspective aspect ratio.
\param[in] nearPlane Specifies the near plane.
\param[in] farPlane Specifies the far plane.
\param[in] fov Specifies the field-of-view angle (in radians).
*/
void prBuildPerspectiveProjection(PRfloat* matrix4x4, PRfloat aspectRatio, PRfloat nearPlane, PRfloat farPlane, PRfloat fov);

/**
Builds a 4x4 left-handed orthogonal projection matrix.
\param[out] matrix4x4 Raw pointer to the resulting 4x4 matrix.
\param[in] width Specifies the orthogonal width.
\param[in] height Specifies the orthogonal height.
\param[in] nearPlane Specifies the near plane.
\param[in] farPlane Specifies the far plane.
*/
void prBuildOrthogonalProjection(PRfloat* matrix4x4, PRfloat width, PRfloat height, PRfloat nearPlane, PRfloat farPlane);

/**
Translates the specified 4x4 left-handed matrix.
\param[in,out] matrix4x4 Raw pointer to the 4x4 matrix which is to be translated.
\param[in] x Specifies the X direction.
\param[in] y Specifies the Y direction.
\param[in] z Specifies the Z direction.
*/
void prTranslate(PRfloat* matrix4x4, PRfloat x, PRfloat y, PRfloat z);

/**
Rotates the specified 4x4 left-handed matrix in a free angle.
\param[in,out] matrix4x4 Raw pointer to the 4x4 matrix which is to be rotated.
\param[in] x Specifies the X rotation axis.
\param[in] y Specifies the Y rotation axis.
\param[in] z Specifies the Z rotation axis.
\param[in] angle Specifies the rotation angle (in radians).
*/
void prRotate(PRfloat* matrix4x4, PRfloat x, PRfloat y, PRfloat z, PRfloat angle);

/**
Scales the specified 4x4 left-handed matrix.
\param[in,out] matrix4x4 Raw pointer to the 4x4 matrix which is to be scaled.
\param[in] x Specifies the width.
\param[in] y Specifies the height.
\param[in] z Specifies the depth.
*/
void prScale(PRfloat* matrix4x4, PRfloat x, PRfloat y, PRfloat z);

/**
Loads the identity of  the specified 4x4 left-handed matrix.
\param[out] matrix4x4 Raw pointer to the 4x4 matrix whose identity is to be loaded.
*/
void prLoadIdentity(PRfloat* matrix4x4);

// --- viewports/ scissors --- //

/**
Sets the viewport for the currently bound framebuffer.
\note A frame buffer must be bound before this function can be used!
\see prBindFrameBuffer
*/
void prViewport(PRuint x, PRuint y, PRuint width, PRuint height);

/**
Sets the depth range for the currently bound framebuffer.
\note A frame buffer must be bound before this function can be used!
\see prBindFrameBuffer
*/
void prDepthRange(PRfloat minDepth, PRfloat maxDepth);

// --- drawing --- //

//! Binds the current color index.
void prColor(PRubyte colorIndex);

//! Draws a single 2D point onto the screen.
void prDrawScreenPoint(PRint x, PRint y, PRubyte colorIndex);

//! Draws a single 2D line onto the screen.
void prDrawScreenLine(PRint x1, PRint y1, PRint x2, PRint y2, PRubyte colorIndex);

//! Draws a single 2D image with the currently bound texture.
void prDrawScreenImage(PRint left, PRint top, PRint right, PRint bottom);

/**
Draws the specified amount of primitives.
\param[in] primitives Specifies the primitive types. This must be PR_PRIMITIVE_...
\param[in] numVertices Specifies the number of vertices to draw.
\param[in] firstVertex Specifies the first vertex to draw.
\remarks A vertex buffer must be bound.
\see prBindVertexBuffer
*/
void prDraw(PRenum primitives, PRushort numVertices, PRushort firstVertex);

/**
Draws the specified amount of primitives.
\param[in] primitives Specifies the primitive types. This must be PR_PRIMITIVE_...
\param[in] numVertices Specifies the number of vertices to draw.
\param[in] firstVertex Specifies the first vertex to draw.
\remarks A vertex buffer and an index buffer must be bound.
\see prBindVertexBuffer
\see prBindIndexBuffer
*/
void prDrawIndexed(PRenum primitives, PRushort numVertices, PRushort firstVertex);


#endif
