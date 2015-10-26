//
//  VDFParser.c
//
//
//  Created by wwwbbat on 15/10/24.
//  Copyright © 2015年 wwwbbat. All rights reserved.
//

#include "VDFParser.h"
#include <string.h>
#include <stdlib.h>

// interface

VDFNode *VDFParse(const void *VDFData, long long length);
void VDFRelease(VDFNode *node);
void _VDFValueRelease(VDFValue *v);

// implementation

VDFNode *
VDFParse(const void *VDFData, long long length)
{
    int size = 2048;
    char *buffer = malloc(size * sizeof(char));
    memset(buffer, 0, size);
    
    int isKey = 1;          //判断 "abcdefg" 之间的 abcdefg 是一个key还是一个value
    int isCharStart = 0;        //判断遇到 " 时，是一个字符串的开始还是结束。
    
    VDFNode *root = NULL;      //拿来返回的根节点
    VDFNode *parent = NULL;    //保存当前父节点
    VDFNode *curNode = NULL;   //保存当前数据的节点

    unsigned char *bytes = (unsigned char *)VDFData;
    int hadAnEscape = 0;   //处理转义字符: "
    
    for (int i=0; i<length; i++) {
        unsigned short  c = bytes[i];
        
        switch (c) {
            case '{':
            {
                if (root == NULL) {     //遇到第一个 { 时，对root赋值
                    root = parent;
                }else{
                    parent = curNode;   //不是第一个{ ，表明需要进入下一层了。
                    curNode = NULL;
                }
                
                isKey = 1;              //下一个完整字符串内的内容将是一个key。
            }
                break;
                
            case '}':
            {
                // 遇到了 }， 表明需要返回上一层了
                
                if (curNode != NULL) {          // data的结尾必定是一个 }。此时curNode已经是NULL了。判断一下防止崩溃
                    curNode->next = NULL;       // next为NULL,代表着这一层所有节点的结束。
                    curNode = parent;           // 返回上一层
                    parent = parent->parent;
                    isKey = 1;                  //下一个完整字符串内内容将是一个key
                }
            }
                break;
                
            case '\"':
            {
                // " 一共有5种情况： key的开头 key的结尾 value的开头 value的结尾， 字符串里的转义字符。
                
                //遇到转义字符时 需要忽略一次 "
                if (hadAnEscape) {
                    hadAnEscape = 0;
                    char tempchar[2] = {c,'\0'};
                    strcat(buffer, tempchar);
                    continue;
                }
                
                isCharStart = !isCharStart;
                if (isCharStart) {
                    // 字符串的开头。清空buffer，拿来保存字符串
                    memset(buffer, 0, size);
                    
                }else{
                    // 字符串的结尾。需要判断是key还是value
                    if (isKey){
                        
                        // 字符串为key时，创建这个key所在的 Node。初始化这个 Node 的Value
                        
                        VDFNode *newNode = malloc(sizeof(VDFNode));
                        memset(newNode, 0, sizeof(VDFNode));
                        VDFValue *v = malloc(sizeof(VDFValue));
                        memset(v, 0, sizeof(VDFValue));
                        
                        if (newNode == NULL) {
                            printf("123123");
                        }
                        
                        if (v == NULL) {
                            printf("asdfasdfa");
                        }
                        
                        char *k = (char *)malloc(strlen(buffer)*sizeof(char));
                        strcpy(k, buffer);
                        newNode->k = k;
                        newNode->v = v;
                        newNode->parent = parent;
                        
                        if (parent == NULL) {
                            parent = newNode;           // 数据最开始的字符串。parent 还未初始化。
                        }else if (curNode == NULL){
                            parent->v->child = newNode;  // 某一层所有节点的起始节点
                            curNode = newNode;
                        }else{
                            curNode->next = newNode;     // 某一层除了起始节点外的其他节点
                            curNode = newNode;
                        }
                    }
                    else{
                        // 字符串为 value。保存字符串到当前节点
                        
                        if (curNode != NULL && curNode->v != NULL) {
                            char *v = (char *)malloc(strlen(buffer)*sizeof(char));
                            strcpy(v, buffer);
                            curNode->v->content = v;
                        }
                    }
                    isKey = !isKey; // 连续出现key-value时，isKey是交替变化的
                }
            }
                break;
            case '\n':case '\r':case '\t':break;    //排除掉无意义的字符
            default:
            {
                if (c == '\\') {
                    hadAnEscape = 1;
                }
                char tempchar[2] = {c,'\0'};    //记录字符到buffer。
                strcat(buffer, tempchar);
            }
                break;
        }
    }
    free(buffer);
    buffer = NULL;
    return root;
}

void
VDFRelease(VDFNode *node){
    
    free(node->k);
    node->k = NULL;
    _VDFValueRelease(node->v);
    node->v = NULL;
    
    if (node->next != NULL) {
        VDFRelease(node->next);
        node->next = NULL;
    }

    free(node);
    node = NULL;
}

void
_VDFValueRelease(VDFValue *v){

    if (v->child) {
        VDFRelease(v->child);
        v->child = NULL;
    }
    if (v->content != NULL) {
        free(v->content);
        v->content = NULL;
    }
}
