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

/**
Returns the specified parameter value.
\param[in] param Specifies the parameter which is to be queried.
- PR_MAX_TEXTURE_SIZE: Returns the maximum texture size.
*/
PRint prGetIntegerv(PRenum param);

// --- context --- //

/**
Generates a new render context. At least one render context is required to render anything.
\param[in] desc Specifies the OS dependent render context description.
\param[in] width Specifies the context width.
\param[in] height Specifies the context height.
\remarks The render context must be deleted with 'prDeleteContext'.
\see prDeleteContext
*/
PRobject prCreateContext(const PRcontextdesc* desc, PRuint width, PRuint height);

/**
Deletes the specified render context.
\param[in] context Specifies the render context which is to be deleted.
This must be generated by 'prCreateContext'.
\see prCreateContext
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
PRobject prCreateFrameBuffer(PRuint width, PRuint height);

/**
Deletes the specified frame buffer.
\param[in] frameBuffer Specifies the frame buffer which is to be deleted.
This must be generated by 'prCreateFrameBuffer'.
\see prCreateFrameBuffer
*/
void prDeleteFrameBuffer(PRobject frameBuffer);

/**
Binds the specified frame buffer.
\param[in] frameBuffer Specifies the frame buffer which is to be bound.
If this is zero, no frame buffer is bound.
\note After a new frame buffer has bound, the viewport and scissor must be set again.
\see prViewport
\see prScissor
*/
void prBindFrameBuffer(PRobject frameBuffer);

/**
Clears the specified frame buffer.
\param[in] frameBuffer Specifies the frame buffer which is to be cleared.
\param[in] clearColor Specifies the color index to clear the color buffer.
\param[in] clearDepth Specifies the depth value to clear the depth buffer.
\param[in] clearFlags Specifies the clear flags. This can be a bitwise OR combination of the following bit masks:
- PR_COLOR_BUFFER_BIT: Clears the color buffer.
- PR_DEPTH_BUFFER_BIT: Clears the depth buffer.
\see prGetColorIndex
*/
void prClearFrameBuffer(PRobject frameBuffer, PRubyte clearColor, PRfloat clearDepth, PRbitfield clearFlags);

//! Converts the specified RGB color into a color index (in the format R3G3B2).
PRubyte prGetColorIndex(PRubyte red, PRubyte green, PRubyte blue);

// --- texture --- //

/**
Generates a new texture.
\return Texture object.
\remarks The texture must be deleted with 'prDeleteTexture'.
\see prDeleteTexture
*/
PRobject prCreateTexture();

/**
Deletes the specified texture.
\param[in] texture Specifies the texture which is to be deleted.
This must be generated by 'prCreateTexture'.
\see prCreateTexture
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
\param[in] format Specifies the image data format. This must be PR_UBYTE_RGB.
\param[in] data Raw pointer to the image data. This must be in the format: PRubyte[width*height*3].
\param[in] dither Specifies whether dithering is to be applied to the image (to compensate 8-bit colors).
\param[in] generateMips Specifies whether MIP maps are to be generated for this texture.
*/
void prTexureImage2D(
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
PRobject prCreateVertexBuffer();

/**
Deletes the specified vertex buffer.
\param[in] vertexBuffer Specifies the vertex buffer which is to be deleted.
This must be generated by 'prCreateVertexBuffer'.
\see prCreateVertexBuffer
*/
void prDeleteVertexBuffer(PRobject vertexBuffer);

/**
Sets the vertex buffer data.
\param[in] vertexBuffer Specifies the vertex buffer whose vertex data is to be set.
\param[in] numVertices Specifies the number of vertices.
\param[in] coords Raw pointer to the vertex coordinates data.
If this is null, the coordinate of all vertices will be initialized with { 0, 0, 0 }.
\param[in] texCoords Raw pointer to the vertex texture coordinates data.
If this is null, the texture coordinate of all vertices will be initialized with { 0, 0 }.
\param[in] vertexStride Specifies the vertex stride size (in bytes).
\code
struct MyVertex
{
    PRfloat x, y, z;    // Vertex coordinates X, Y, Z
    PRfloat u, v;       // Texture coordinates U, V
};
struct MyVertex[32] = ...
prVertexBufferData(vertexBuffer, 32, &(MyVertex[0].x), &(MyVertex[0].u), sizeof(MyVertex));
\endcode
*/
void prVertexBufferData(PRobject vertexBuffer, PRsizei numVertices, const PRvoid* coords, const PRvoid* texCoords, PRsizei vertexStride);

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
void prVertexBufferDataFromFile(PRobject vertexBuffer, PRsizei* numVertices, FILE* file);

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
PRobject prCreateIndexBuffer();

/**
Deletes the specified index buffer.
\param[in] indexBuffer Specifies the index buffer which is to be deleted.
This must be generated by 'prCreateIndexBuffer'.
\see prCreateIndexBuffer
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
void prIndexBufferDataFromFile(PRobject indexBuffer, PRsizei* numIndices, FILE* file);

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

// --- states --- //

/**
Sets the specified state.
\param[in] cap Specifies the capability whose state is to be changed.
Value values are:
- PR_SCISSOR - Enables/disables the scissor rectangle (see prScissor). By default PR_FALSE.
\param[in] state Specifies the new state.
\see prEnable
\see prDisable
*/
void prSetState(PRenum cap, PRboolean state);

/**
Returns the current value of the specified state.
\param[in] cap Specifies the capability whose state is to be returned.
\see prSetState
*/
PRboolean prGetState(PRenum cap);

/**
Enables the specified capability. This is equivalent to:
\code
prSetState(cap, PR_TRUE);
\endcode
\see prSetState
*/
void prEnable(PRenum cap);

/**
Disables the specified capability. This is equivalent to:
\code
prSetState(cap, PR_FALSE);
\endcode
\see prSetState
*/
void prDisable(PRenum cap);

/**
Sets the viewport for the currently bound framebuffer.
\note A frame buffer must be bound before this function can be used!
\see prBindFrameBuffer
*/
void prViewport(PRint x, PRint y, PRint width, PRint height);

/**
Sets the scissor rectangle for the currently bound framebuffer.
\note A frame buffer must be bound before this function can be used!
\see prBindFrameBuffer
*/
void prScissor(PRint x, PRint y, PRint width, PRint height);

/**
Sets the depth range for the currently bound framebuffer.
\note A frame buffer must be bound before this function can be used!
\see prBindFrameBuffer
*/
void prDepthRange(PRfloat minDepth, PRfloat maxDepth);

/**
Sets the face culling mode.
\param[in] mode Specifies the new culling mode.
This must be PR_CULL_NONE, PR_CULL_FRONT or PR_CULL_BACK.
By default PR_CULL_NONE.
*/
void prCullMode(PRenum mode);

/**
Sets the polygon drawing mode.
\param[in] mode Specifies the new polygon mode.
This must be PR_POLYGON_FILL, PR_POLYGON_LINE or PR_POLYGON_POINT.
By default PR_POLYGON_FILL.
*/
void prPolygonMode(PRenum mode);

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
\param[in] primitives Specifies the primitive types. Valid values are:
PR_POINTS, PR_LINES, PR_LINE_STRIP, PR_LINE_LOOP, PR_TRIANGLES, PR_TRIANGLE_STRIP, PR_TRIANGLE_FAN.
\param[in] numVertices Specifies the number of vertices to draw.
\param[in] firstVertex Specifies the first vertex to draw.
\remarks A vertex buffer must be bound.
\see prBindVertexBuffer
*/
void prDraw(PRenum primitives, PRushort numVertices, PRushort firstVertex);

/**
Draws the specified amount of primitives.
\param[in] primitives Specifies the primitive types. Valid values are:
PR_POINTS, PR_LINES, PR_LINE_STRIP, PR_LINE_LOOP, PR_TRIANGLES, PR_TRIANGLE_STRIP, PR_TRIANGLE_FAN.
\param[in] numVertices Specifies the number of vertices to draw.
\param[in] firstVertex Specifies the first vertex to draw.
\remarks A vertex buffer and an index buffer must be bound.
\see prBindVertexBuffer
\see prBindIndexBuffer
*/
void prDrawIndexed(PRenum primitives, PRushort numVertices, PRushort firstVertex);

// --- immediate mode --- //

/**
Begins the immediate drawing mode.
\param[in] primitives Specifies the primitive types. Valid values are:
PR_POINTS, PR_LINES, PR_LINE_STRIP, PR_LINE_LOOP, PR_TRIANGLES, PR_TRIANGLE_STRIP, PR_TRIANGLE_FAN.
\remarks This must be finished by calling "prEnd". Here is a usage example:
\code
// Draw a quad
prBegin(PR_TRIANGLE_STRIP);
{
    prTexCoord2i(0, 0);
    prVertex2i(-1, 1);

    prTexCoord2i(1, 0);
    prVertex2i(1, 1);

    prTexCoord2i(0, 1);
    prVertex2i(-1, -1);

    prTexCoord2i(1, 1);
    prVertex2i(1, -1);
}
prEnd();
\encode
\remarks This is equivalent to drawing the primitives with a vertex buffer (but no index buffer).
\note This is slower than using a vertex buffer. A global immediate vertex buffer is used,
to draw the primitives. This internal global buffer is severely limited (by default 32 vertices).
However, you can drawn unlimited primitives, since the buffer works like a stream,
which will be flushed when it's full (i.e. the current content will be drawn to the frame buffer).
\see prEnd
\see prDraw
*/
void prBegin(PRenum primitives);
/**
Ends the immediate drawing mode.
\remarks This must be started by calling "prBegin".
\see prBegin
*/
void prEnd();

/**
Sets the texture coordinates of the current vertex in the immediate drawing mode.
\remarks This must be called between "prBegin" and "prEnd".
\see prBegin
\see prEnd
*/
void prTexCoord2f(PRfloat u, PRfloat v);

// \see prTexCoord2f
void prTexCoord2i(PRint u, PRint v);

/**
Sets the coordinates of the current vertex in the immediate drawing mode.
This function will also increment the vertex counter,
i.e. the next calls to "prTexCoord..." will configure the next vertex.
Thus call "prVertex..." only when all configurations for the current vertex are done.
\remarks This must be called between "prBegin" and "prEnd".
\see prBegin
\see prEnd
*/
void prVertex4f(PRfloat x, PRfloat y, PRfloat z, PRfloat w);

//! \see prVertex4f
void prVertex4i(PRint x, PRint y, PRint z, PRint w);

/**
Sets the coordinates of the current vertex. The 'w' component will be set to 1.0.
\see prVertex4f
*/
void prVertex3f(PRfloat x, PRfloat y, PRfloat z);

//! \see prVertex3f
void prVertex3i(PRint x, PRint y, PRint z);

/**
Sets the coordinates of the current vertex.
The 'z' component will be set to 0.0 and the 'w' component will be set to 1.0.
\see prVertex4f
*/
void prVertex2f(PRfloat x, PRfloat y);

//! \see prVertex2f
void prVertex2i(PRint x, PRint y);


#endif
