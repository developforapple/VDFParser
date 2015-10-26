//
//  VDFParser.h
//  
//
//  Created by wwwbbat on 15/10/24.
//  Copyright © 2015年 wwwbbat. All rights reserved.
//

#ifndef VDFParser_h
#define VDFParser_h

#include <stdio.h>

typedef struct VDFNode{
    char *k;                //key
    struct VDFValue *v;     //value
    
    struct VDFNode *next;   // next sibling Node
    struct VDFNode *parent; // parent Node
    
} VDFNode;

typedef struct VDFValue{    // content and child are mutually exclusive
    char *content;          // if not NULL, value is string. No children.
    struct VDFNode *child;  // if not NULL, the Node has child nodes
} VDFValue;

VDFNode *VDFParse(const void *VDFData, long long length);
void VDFRelease(VDFNode *node);

#endif /* VDFParser_h */
