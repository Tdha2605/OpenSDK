/*
  Copyright (c) 2009-2017 Dave Gamble and  Quos_cJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef __QUOS_CJSON_H__
#define __QUOS_CJSON_H__

#include "quos_config.h"
#if (SDK_ENABLE_JSON == 1)
/* The  Quos_cJSON structure: */
typedef struct Quos_cJSON
{
  /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
  struct Quos_cJSON *next;
  struct Quos_cJSON *prev;
  /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
  struct Quos_cJSON *child;

  /* The type of the item, as above. */
  int type;

  /* The item's string, if type==QUOS_cJSON_String  and type == QUOS_cJSON_Raw */
  char *valuestring;
  /* writing to valueint is DEPRECATED, use Quos_cJSON_SetNumberValue instead */
  int valueint;
  /* The item's number, if type==QUOS_cJSON_Number */
  double valuedouble;

  /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
  char *string;
} Quos_cJSON;

/* returns the version of  Quos_cJSON as a string */
const char *Quos_cJSON_Version(void);
/* Supply a block of JSON, and this returns a  Quos_cJSON object you can interrogate. */
Quos_cJSON *Quos_cJSON_Parse(const char *value);
Quos_cJSON *Quos_cJSON_ParseWithLength(const char *value, quint32_t buffer_length);
/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
/* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain a pointer to the error so will match Quos_cJSON_GetErrorPtr(). */
Quos_cJSON *Quos_cJSON_ParseWithOpts(const char *value, const char **return_parse_end, qbool require_null_terminated);
Quos_cJSON *Quos_cJSON_ParseWithLengthOpts(const char *value, quint32_t buffer_length, const char **return_parse_end, qbool require_null_terminated);

/* Render a  Quos_cJSON entity to text for transfer/storage. */
char *Quos_cJSON_Print(const Quos_cJSON *item);
/* Render a  Quos_cJSON entity to text for transfer/storage without any formatting. */
char *Quos_cJSON_PrintUnformatted(const Quos_cJSON *item);
/* Render a  Quos_cJSON entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
char *Quos_cJSON_PrintBuffered(const Quos_cJSON *item, int prebuffer, qbool fmt);
/* Render a  Quos_cJSON entity to text using a buffer already allocated in memory with given length. Returns 1 on success and 0 on failure. */
/* NOTE:  Quos_cJSON is not always 100% accurate in estimating how much memory it will use, so to be safe allocate 5 bytes more than you actually need */
qbool Quos_cJSON_PrintPreallocated(Quos_cJSON *item, char *buffer, const int length, const qbool format);
/* Delete a  Quos_cJSON entity and all subentities. */
void Quos_cJSON_Delete(Quos_cJSON *item);

/* Returns the number of items in an array (or object). */
int Quos_cJSON_GetArraySize(const Quos_cJSON *array);
/* Retrieve item number "index" from array "array". Returns NULL if unsuccessful. */
Quos_cJSON *Quos_cJSON_GetArrayItem(const Quos_cJSON *array, int index);
/* Get item "string" from object. Case insensitive. */
Quos_cJSON *Quos_cJSON_GetObjectItem(const Quos_cJSON *const object, const char *const string);
Quos_cJSON *Quos_cJSON_GetObjectItemCaseSensitive(const Quos_cJSON *const object, const char *const string);
qbool Quos_cJSON_HasObjectItem(const Quos_cJSON *object, const char *string);
/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when Quos_cJSON_Parse() returns 0. 0 when Quos_cJSON_Parse() succeeds. */
const char *Quos_cJSON_GetErrorPtr(void);

/* Check item type and return its value */
char *Quos_cJSON_GetStringValue(const Quos_cJSON *const item);
double Quos_cJSON_GetNumberValue(const Quos_cJSON *const item);

/* These functions check the type of an item */
qbool Quos_cJSON_IsInvalid(const Quos_cJSON *const item);
qbool Quos_cJSON_IsFalse(const Quos_cJSON *const item);
qbool Quos_cJSON_IsTrue(const Quos_cJSON *const item);
qbool Quos_cJSON_IsBool(const Quos_cJSON *const item);
qbool Quos_cJSON_IsNull(const Quos_cJSON *const item);
qbool Quos_cJSON_IsNumber(const Quos_cJSON *const item);
qbool Quos_cJSON_IsString(const Quos_cJSON *const item);
qbool Quos_cJSON_IsArray(const Quos_cJSON *const item);
qbool Quos_cJSON_IsObject(const Quos_cJSON *const item);
qbool Quos_cJSON_IsRaw(const Quos_cJSON *const item);

/* These calls create a  Quos_cJSON item of the appropriate type. */
Quos_cJSON *Quos_cJSON_CreateNull(void);
Quos_cJSON *Quos_cJSON_CreateTrue(void);
Quos_cJSON *Quos_cJSON_CreateFalse(void);
Quos_cJSON *Quos_cJSON_CreateBool(qbool boolean);
Quos_cJSON *Quos_cJSON_CreateNumber(double num);
Quos_cJSON *Quos_cJSON_CreateString(const char *string);
/* raw json */
Quos_cJSON *Quos_cJSON_CreateRaw(const char *raw);
Quos_cJSON *Quos_cJSON_CreateArray(void);
Quos_cJSON *Quos_cJSON_CreateObject(void);

/* Create a string where valuestring references a string so
 * it will not be freed by Quos_cJSON_Delete */
Quos_cJSON *Quos_cJSON_CreateStringReference(const char *string);
/* Create an object/array that only references it's elements so
 * they will not be freed by Quos_cJSON_Delete */
Quos_cJSON *Quos_cJSON_CreateObjectReference(const Quos_cJSON *child);
Quos_cJSON *Quos_cJSON_CreateArrayReference(const Quos_cJSON *child);

/* These utilities create an Array of count items.
 * The parameter count cannot be greater than the number of elements in the number array, otherwise array access will be out of bounds.*/
Quos_cJSON *Quos_cJSON_CreateIntArray(const int *numbers, int count);
Quos_cJSON *Quos_cJSON_CreateFloatArray(const float *numbers, int count);
Quos_cJSON *Quos_cJSON_CreateDoubleArray(const double *numbers, int count);
Quos_cJSON *Quos_cJSON_CreateStringArray(const char *const *strings, int count);

/* Append item to the specified array/object. */
qbool Quos_cJSON_AddItemToArray(Quos_cJSON *array, Quos_cJSON *item);
qbool Quos_cJSON_AddItemToObject(Quos_cJSON *object, const char *string, Quos_cJSON *item);
/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the  Quos_cJSON object.
 * WARNING: When this function was used, make sure to always check that (item->type & QUOS_cJSON_StringIsConst) is zero before
 * writing to `item->string` */
qbool Quos_cJSON_AddItemToObjectCS(Quos_cJSON *object, const char *string, Quos_cJSON *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing  Quos_cJSON to a new  Quos_cJSON, but don't want to corrupt your existing  Quos_cJSON. */
qbool Quos_cJSON_AddItemReferenceToArray(Quos_cJSON *array, Quos_cJSON *item);
qbool Quos_cJSON_AddItemReferenceToObject(Quos_cJSON *object, const char *string, Quos_cJSON *item);

/* Remove/Detach items from Arrays/Objects. */
Quos_cJSON *Quos_cJSON_DetachItemViaPointer(Quos_cJSON *parent, Quos_cJSON *const item);
Quos_cJSON *Quos_cJSON_DetachItemFromArray(Quos_cJSON *array, int which);
void Quos_cJSON_DeleteItemFromArray(Quos_cJSON *array, int which);
Quos_cJSON *Quos_cJSON_DetachItemFromObject(Quos_cJSON *object, const char *string);
Quos_cJSON *Quos_cJSON_DetachItemFromObjectCaseSensitive(Quos_cJSON *object, const char *string);
void Quos_cJSON_DeleteItemFromObject(Quos_cJSON *object, const char *string);
void Quos_cJSON_DeleteItemFromObjectCaseSensitive(Quos_cJSON *object, const char *string);

/* Update array items. */
qbool Quos_cJSON_InsertItemInArray(Quos_cJSON *array, int which, Quos_cJSON *newitem); /* Shifts pre-existing items to the right. */
qbool Quos_cJSON_ReplaceItemViaPointer(Quos_cJSON *const parent, Quos_cJSON *const item, Quos_cJSON *replacement);
qbool Quos_cJSON_ReplaceItemInArray(Quos_cJSON *array, int which, Quos_cJSON *newitem);
qbool Quos_cJSON_ReplaceItemInObject(Quos_cJSON *object, const char *string, Quos_cJSON *newitem);
qbool Quos_cJSON_ReplaceItemInObjectCaseSensitive(Quos_cJSON *object, const char *string, Quos_cJSON *newitem);

/* Duplicate a  Quos_cJSON item */
Quos_cJSON *Quos_cJSON_Duplicate(const Quos_cJSON *item, qbool recurse);
/* Duplicate will create a new, identical  Quos_cJSON item to the one you pass, in new memory that will
 * need to be released. With recurse!=0, it will duplicate any children connected to the item.
 * The item->next and ->prev pointers are always zero on return from Duplicate. */
/* Recursively compare two  Quos_cJSON items for equality. If either a or b is NULL or invalid, they will be considered unequal.
 * case_sensitive determines if object keys are treated case sensitive (1) or case insensitive (0) */
qbool Quos_cJSON_Compare(const Quos_cJSON *const a, const Quos_cJSON *const b, const qbool case_sensitive);

/* Minify a strings, remove blank characters(such as ' ', '\t', '\r', '\n') from strings.
 * The input pointer json cannot point to a read-only address area, such as a string constant,
 * but should point to a readable and writable adress area. */
void Quos_cJSON_Minify(char *json);

/* Helper functions for creating and adding items to an object at the same time.
 * They return the added item or NULL on failure. */
Quos_cJSON *Quos_cJSON_AddNullToObject(Quos_cJSON *const object, const char *const name);
Quos_cJSON *Quos_cJSON_AddTrueToObject(Quos_cJSON *const object, const char *const name);
Quos_cJSON *Quos_cJSON_AddFalseToObject(Quos_cJSON *const object, const char *const name);
Quos_cJSON *Quos_cJSON_AddBoolToObject(Quos_cJSON *const object, const char *const name, const qbool boolean);
Quos_cJSON *Quos_cJSON_AddNumberToObject(Quos_cJSON *const object, const char *const name, const double number);
Quos_cJSON *Quos_cJSON_AddStringToObject(Quos_cJSON *const object, const char *const name, const char *const string);
Quos_cJSON *Quos_cJSON_AddRawToObject(Quos_cJSON *const object, const char *const name, const char *const raw);
Quos_cJSON *Quos_cJSON_AddObjectToObject(Quos_cJSON *const object, const char *const name);
Quos_cJSON *Quos_cJSON_AddArrayToObject(Quos_cJSON *const object, const char *const name);

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define Quos_cJSON_SetIntValue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
/* helper for the Quos_cJSON_SetNumberValue macro */
double Quos_cJSON_SetNumberHelper(Quos_cJSON *object, double number);
#define Quos_cJSON_SetNumberValue(object, number) ((object != NULL) ? Quos_cJSON_SetNumberHelper(object, (double)number) : (number))
/* Change the valuestring of a QUOS_cJSON_String object, only takes effect when type of object is QUOS_cJSON_String */
char *Quos_cJSON_SetValuestring(Quos_cJSON *object, const char *valuestring);

/* Macro for iterating over an array or object */
#define QUOS_cJSON_ArrayForEach(element, array) for (element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

#elif (SDK_ENABLE_JSON == 2)
#define Quos_cJSON cJSON
#define Quos_cJSON_Version cJSON_Version
#define Quos_cJSON_Parse cJSON_Parse
#define Quos_cJSON_ParseWithLength cJSON_ParseWithLength
#define Quos_cJSON_ParseWithOpts cJSON_ParseWithOpts
#define Quos_cJSON_ParseWithLengthOpts cJSON_ParseWithLengthOpts
#define Quos_cJSON_Print cJSON_Print
#define Quos_cJSON_PrintUnformatted cJSON_PrintUnformatted
#define Quos_cJSON_PrintBuffered cJSON_PrintBuffered
#define Quos_cJSON_PrintPreallocated cJSON_PrintPreallocated
#define Quos_cJSON_Delete cJSON_Delete

#define Quos_cJSON_GetArraySize cJSON_GetArraySize
#define Quos_cJSON_GetArrayItem cJSON_GetArrayItem
#define Quos_cJSON_GetObjectItem cJSON_GetObjectItem
#define Quos_cJSON_GetObjectItemCaseSensitive cJSON_GetObjectItemCaseSensitive
#define Quos_cJSON_HasObjectItem cJSON_HasObjectItem
#define Quos_cJSON_GetErrorPtr cJSON_GetErrorPtr
#define Quos_cJSON_GetStringValue cJSON_GetStringValue
#define Quos_cJSON_GetNumberValue cJSON_GetNumberValue

#define Quos_cJSON_IsInvalid cJSON_IsInvalid
#define Quos_cJSON_IsFalse cJSON_IsFalse
#define Quos_cJSON_IsTrue cJSON_IsTrue
#define Quos_cJSON_IsBool cJSON_IsBool
#define Quos_cJSON_IsNull cJSON_IsNull
#define Quos_cJSON_IsNumber cJSON_IsNumber
#define Quos_cJSON_IsString cJSON_IsString
#define Quos_cJSON_IsArray cJSON_IsArray
#define Quos_cJSON_IsObject cJSON_IsObject
#define Quos_cJSON_IsRaw cJSON_IsRaw

#define Quos_cJSON_CreateNull cJSON_CreateNull
#define Quos_cJSON_CreateTrue cJSON_CreateTrue
#define Quos_cJSON_CreateFalse cJSON_CreateFalse
#define Quos_cJSON_CreateBool cJSON_CreateBool
#define Quos_cJSON_CreateNumber cJSON_CreateNumber
#define Quos_cJSON_CreateString cJSON_CreateString
#define Quos_cJSON_CreateRaw cJSON_CreateRaw
#define Quos_cJSON_CreateArray cJSON_CreateArray
#define Quos_cJSON_CreateObject cJSON_CreateObject
#define Quos_cJSON_CreateStringReference cJSON_CreateStringReference
#define Quos_cJSON_CreateObjectReference cJSON_CreateObjectReference
#define Quos_cJSON_CreateArrayReference cJSON_CreateArrayReference
#define Quos_cJSON_CreateIntArray cJSON_CreateIntArray
#define Quos_cJSON_CreateFloatArray cJSON_CreateFloatArray
#define Quos_cJSON_CreateDoubleArray cJSON_CreateDoubleArray
#define Quos_cJSON_CreateStringArray cJSON_CreateStringArray
#define Quos_cJSON_AddItemToArray cJSON_AddItemToArray
#define Quos_cJSON_AddItemToObject cJSON_AddItemToObject
#define Quos_cJSON_AddItemToObjectCS cJSON_AddItemToObjectCS
#define Quos_cJSON_AddItemReferenceToArray cJSON_AddItemReferenceToArray
#define Quos_cJSON_AddItemReferenceToObject cJSON_AddItemReferenceToObject
#define Quos_cJSON_DetachItemViaPointer cJSON_DetachItemViaPointer
#define Quos_cJSON_DetachItemFromArray cJSON_DetachItemFromArray
#define Quos_cJSON_DeleteItemFromArray cJSON_DeleteItemFromArray
#define Quos_cJSON_DetachItemFromObject cJSON_DetachItemFromObject
#define Quos_cJSON_DetachItemFromObjectCaseSensitive cJSON_DetachItemFromObjectCaseSensitive
#define Quos_cJSON_DeleteItemFromObject cJSON_DeleteItemFromObject
#define Quos_cJSON_DeleteItemFromObjectCaseSensitive cJSON_DeleteItemFromObjectCaseSensitive
#define Quos_cJSON_InsertItemInArray cJSON_InsertItemInArray
#define Quos_cJSON_ReplaceItemViaPointer cJSON_ReplaceItemViaPointer
#define Quos_cJSON_ReplaceItemInArray cJSON_ReplaceItemInArray
#define Quos_cJSON_ReplaceItemInObject cJSON_ReplaceItemInObject
#define Quos_cJSON_ReplaceItemInObjectCaseSensitive cJSON_ReplaceItemInObjectCaseSensitive
#define Quos_cJSON_Duplicate cJSON_Duplicate
#define Quos_cJSON_Compare cJSON_Compare
#define Quos_cJSON_Minify cJSON_Minify

#define Quos_cJSON_AddNullToObject cJSON_AddNullToObject
#define Quos_cJSON_AddTrueToObject cJSON_AddTrueToObject
#define Quos_cJSON_AddFalseToObject cJSON_AddFalseToObject
#define Quos_cJSON_AddBoolToObject cJSON_AddBoolToObject
#define Quos_cJSON_AddNumberToObject cJSON_AddNumberToObject
#define Quos_cJSON_AddStringToObject cJSON_AddStringToObject
#define Quos_cJSON_AddRawToObject cJSON_AddRawToObject
#define Quos_cJSON_AddObjectToObject cJSON_AddObjectToObject
#define Quos_cJSON_AddArrayToObject cJSON_AddArrayToObject

#define Quos_cJSON_SetIntValue cJSON_SetIntValue
#define Quos_cJSON_SetNumberHelper cJSON_SetNumberHelper
#define Quos_cJSON_SetNumberHelper cJSON_SetNumberHelper
#define Quos_cJSON_SetValuestring cJSON_SetValuestring
#define QUOS_cJSON_ArrayForEach cJSON_ArrayForEach
#endif/*SDK_ENABLE_JSON */

#endif/*__QUOS_CJSON_H__ */
