/*
 * vertex.h
 * 
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_VERTEX_H__
#define __PR_VERTEX_H__


#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix4.h"
#include "static_config.h"


//! Simple vertex with 3D coordinate and 2D texture-coordinate.
typedef struct pr_vertex
{
    // Before vertex processing
    pr_vector4 coord;       //!< Original coordinate.
    pr_vector2 texCoord;    //!< Texture-coordinate.

    #if 1//!remove!
    pr_vector4 ndc;         //!< Normalized device coordinate.
    #ifdef PR_PERSPECTIVE_CORRECTED
    pr_vector2 invTexCoord; //!< Inverse texture-coordinates.
    #endif
    #endif
}
pr_vertex;


void _pr_vertex_init(pr_vertex* vertex);


#endif
