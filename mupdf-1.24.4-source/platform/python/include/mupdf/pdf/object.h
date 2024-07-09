// Copyright (C) 2004-2021 Artifex Software, Inc.
//
// This file is part of MuPDF.
//
// MuPDF is free software: you can redistribute it and/or modify it under the
// terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// MuPDF is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
// details.
//
// You should have received a copy of the GNU Affero General Public License
// along with MuPDF. If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
//
// Alternative licensing terms are available from the licensor.
// For commercial licensing, see <https://www.artifex.com/> or contact
// Artifex Software, Inc., 39 Mesa Street, Suite 108A, San Francisco,
// CA 94129, USA, for further information.

#ifndef MUPDF_PDF_OBJECT_H
#define MUPDF_PDF_OBJECT_H

#include "mupdf/fitz/stream.h"

typedef struct pdf_document pdf_document;
typedef struct pdf_crypt pdf_crypt;
typedef struct pdf_journal pdf_journal;

/* Defined in PDF 1.7 according to Acrobat limit. */
#define PDF_MAX_OBJECT_NUMBER 8388607
#define PDF_MAX_GEN_NUMBER 65535

/*
 * Dynamic objects.
 * The same type of objects as found in PDF and PostScript.
 * Used by the filters and the mupdf parser.
 */

typedef struct pdf_obj pdf_obj;

pdf_obj *pdf_new_int(fz_context *ctx, int64_t i);
pdf_obj *pdf_new_real(fz_context *ctx, float f);
pdf_obj *pdf_new_name(fz_context *ctx, const char *str);
pdf_obj *pdf_new_string(fz_context *ctx, const char *str, size_t len);

/*
	Create a PDF 'text string' by encoding input string as either ASCII or UTF-16BE.
	In theory, we could also use PDFDocEncoding.
*/
pdf_obj *pdf_new_text_string(fz_context *ctx, const char *s);
pdf_obj *pdf_new_indirect(fz_context *ctx, pdf_document *doc, int num, int gen);
pdf_obj *pdf_new_array(fz_context *ctx, pdf_document *doc, int initialcap);
pdf_obj *pdf_new_dict(fz_context *ctx, pdf_document *doc, int initialcap);
pdf_obj *pdf_new_rect(fz_context *ctx, pdf_document *doc, fz_rect rect);
pdf_obj *pdf_new_matrix(fz_context *ctx, pdf_document *doc, fz_matrix mtx);
pdf_obj *pdf_new_date(fz_context *ctx, pdf_document *doc, int64_t time);
pdf_obj *pdf_copy_array(fz_context *ctx, pdf_obj *array);
pdf_obj *pdf_copy_dict(fz_context *ctx, pdf_obj *dict);
pdf_obj *pdf_deep_copy_obj(fz_context *ctx, pdf_obj *obj);

pdf_obj *pdf_keep_obj(fz_context *ctx, pdf_obj *obj);
void pdf_drop_obj(fz_context *ctx, pdf_obj *obj);
pdf_obj *pdf_drop_singleton_obj(fz_context *ctx, pdf_obj *obj);

int pdf_is_null(fz_context *ctx, pdf_obj *obj);
int pdf_is_bool(fz_context *ctx, pdf_obj *obj);
int pdf_is_int(fz_context *ctx, pdf_obj *obj);
int pdf_is_real(fz_context *ctx, pdf_obj *obj);
int pdf_is_number(fz_context *ctx, pdf_obj *obj);
int pdf_is_name(fz_context *ctx, pdf_obj *obj);
int pdf_is_string(fz_context *ctx, pdf_obj *obj);
int pdf_is_array(fz_context *ctx, pdf_obj *obj);
int pdf_is_dict(fz_context *ctx, pdf_obj *obj);
int pdf_is_indirect(fz_context *ctx, pdf_obj *obj);

/*
	Check if an object is a stream or not.
*/
int pdf_obj_num_is_stream(fz_context *ctx, pdf_document *doc, int num);
int pdf_is_stream(fz_context *ctx, pdf_obj *obj);

/* Compare 2 objects. Returns 0 on match, non-zero on mismatch.
 * Streams always mismatch.
 */
int pdf_objcmp(fz_context *ctx, pdf_obj *a, pdf_obj *b);
int pdf_objcmp_resolve(fz_context *ctx, pdf_obj *a, pdf_obj *b);

/* Compare 2 objects. Returns 0 on match, non-zero on mismatch.
 * Stream contents are explicitly checked.
 */
int pdf_objcmp_deep(fz_context *ctx, pdf_obj *a, pdf_obj *b);

int pdf_name_eq(fz_context *ctx, pdf_obj *a, pdf_obj *b);

int pdf_obj_marked(fz_context *ctx, pdf_obj *obj);
int pdf_mark_obj(fz_context *ctx, pdf_obj *obj);
void pdf_unmark_obj(fz_context *ctx, pdf_obj *obj);

typedef struct pdf_cycle_list pdf_cycle_list;
struct pdf_cycle_list {
	pdf_cycle_list *up;
	int num;
};
int pdf_cycle(fz_context *ctx, pdf_cycle_list *here, pdf_cycle_list *prev, pdf_obj *obj);

typedef struct
{
	int len;
	unsigned char bits[1];
} pdf_mark_bits;

pdf_mark_bits *pdf_new_mark_bits(fz_context *ctx, pdf_document *doc);
void pdf_drop_mark_bits(fz_context *ctx, pdf_mark_bits *marks);
void pdf_mark_bits_reset(fz_context *ctx, pdf_mark_bits *marks);
int pdf_mark_bits_set(fz_context *ctx, pdf_mark_bits *marks, pdf_obj *obj);

typedef struct
{
	int len;
	int max;
	int *list;
	int local_list[8];
} pdf_mark_list;

int pdf_mark_list_push(fz_context *ctx, pdf_mark_list *list, pdf_obj *obj);
void pdf_mark_list_pop(fz_context *ctx, pdf_mark_list *list);
int pdf_mark_list_check(fz_context *ctx, pdf_mark_list *list, pdf_obj *obj);
void pdf_mark_list_init(fz_context *ctx, pdf_mark_list *list);
void pdf_mark_list_free(fz_context *ctx, pdf_mark_list *list);

void pdf_set_obj_memo(fz_context *ctx, pdf_obj *obj, int bit, int memo);
int pdf_obj_memo(fz_context *ctx, pdf_obj *obj, int bit, int *memo);

int pdf_obj_is_dirty(fz_context *ctx, pdf_obj *obj);
void pdf_dirty_obj(fz_context *ctx, pdf_obj *obj);
void pdf_clean_obj(fz_context *ctx, pdf_obj *obj);

int pdf_to_bool(fz_context *ctx, pdf_obj *obj);
int pdf_to_int(fz_context *ctx, pdf_obj *obj);
int64_t pdf_to_int64(fz_context *ctx, pdf_obj *obj);
float pdf_to_real(fz_context *ctx, pdf_obj *obj);
const char *pdf_to_name(fz_context *ctx, pdf_obj *obj);
const char *pdf_to_text_string(fz_context *ctx, pdf_obj *obj);
const char *pdf_to_string(fz_context *ctx, pdf_obj *obj, size_t *sizep);
char *pdf_to_str_buf(fz_context *ctx, pdf_obj *obj);
size_t pdf_to_str_len(fz_context *ctx, pdf_obj *obj);
int pdf_to_num(fz_context *ctx, pdf_obj *obj);
int pdf_to_gen(fz_context *ctx, pdf_obj *obj);

int pdf_to_bool_default(fz_context *ctx, pdf_obj *obj, int def);
int pdf_to_int_default(fz_context *ctx, pdf_obj *obj, int def);
float pdf_to_real_default(fz_context *ctx, pdf_obj *obj, float def);

int pdf_array_len(fz_context *ctx, pdf_obj *array);
pdf_obj *pdf_array_get(fz_context *ctx, pdf_obj *array, int i);
void pdf_array_put(fz_context *ctx, pdf_obj *array, int i, pdf_obj *obj);
void pdf_array_put_drop(fz_context *ctx, pdf_obj *array, int i, pdf_obj *obj);
void pdf_array_push(fz_context *ctx, pdf_obj *array, pdf_obj *obj);
void pdf_array_push_drop(fz_context *ctx, pdf_obj *array, pdf_obj *obj);
void pdf_array_insert(fz_context *ctx, pdf_obj *array, pdf_obj *obj, int index);
void pdf_array_insert_drop(fz_context *ctx, pdf_obj *array, pdf_obj *obj, int index);
void pdf_array_delete(fz_context *ctx, pdf_obj *array, int index);
int pdf_array_find(fz_context *ctx, pdf_obj *array, pdf_obj *obj);
int pdf_array_contains(fz_context *ctx, pdf_obj *array, pdf_obj *obj);

int pdf_dict_len(fz_context *ctx, pdf_obj *dict);
pdf_obj *pdf_dict_get_key(fz_context *ctx, pdf_obj *dict, int idx);
pdf_obj *pdf_dict_get_val(fz_context *ctx, pdf_obj *dict, int idx);
void pdf_dict_put_val_null(fz_context *ctx, pdf_obj *obj, int idx);
pdf_obj *pdf_dict_get(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
pdf_obj *pdf_dict_getp(fz_context *ctx, pdf_obj *dict, const char *path);
pdf_obj *pdf_dict_getl(fz_context *ctx, pdf_obj *dict, ...);
pdf_obj *pdf_dict_geta(fz_context *ctx, pdf_obj *dict, pdf_obj *key, pdf_obj *abbrev);
pdf_obj *pdf_dict_gets(fz_context *ctx, pdf_obj *dict, const char *key);
pdf_obj *pdf_dict_getsa(fz_context *ctx, pdf_obj *dict, const char *key, const char *abbrev);
pdf_obj *pdf_dict_get_inheritable(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
pdf_obj *pdf_dict_getp_inheritable(fz_context *ctx, pdf_obj *dict, const char *path);
pdf_obj *pdf_dict_gets_inheritable(fz_context *ctx, pdf_obj *dict, const char *key);
void pdf_dict_put(fz_context *ctx, pdf_obj *dict, pdf_obj *key, pdf_obj *val);
void pdf_dict_put_drop(fz_context *ctx, pdf_obj *dict, pdf_obj *key, pdf_obj *val);
void pdf_dict_get_put_drop(fz_context *ctx, pdf_obj *dict, pdf_obj *key, pdf_obj *val, pdf_obj **old_val);
void pdf_dict_puts(fz_context *ctx, pdf_obj *dict, const char *key, pdf_obj *val);
void pdf_dict_puts_drop(fz_context *ctx, pdf_obj *dict, const char *key, pdf_obj *val);
void pdf_dict_putp(fz_context *ctx, pdf_obj *dict, const char *path, pdf_obj *val);
void pdf_dict_putp_drop(fz_context *ctx, pdf_obj *dict, const char *path, pdf_obj *val);
void pdf_dict_putl(fz_context *ctx, pdf_obj *dict, pdf_obj *val, ...);
void pdf_dict_putl_drop(fz_context *ctx, pdf_obj *dict, pdf_obj *val, ...);
void pdf_dict_del(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
void pdf_dict_dels(fz_context *ctx, pdf_obj *dict, const char *key);
void pdf_sort_dict(fz_context *ctx, pdf_obj *dict);

void pdf_dict_put_bool(fz_context *ctx, pdf_obj *dict, pdf_obj *key, int x);
void pdf_dict_put_int(fz_context *ctx, pdf_obj *dict, pdf_obj *key, int64_t x);
void pdf_dict_put_real(fz_context *ctx, pdf_obj *dict, pdf_obj *key, double x);
void pdf_dict_put_name(fz_context *ctx, pdf_obj *dict, pdf_obj *key, const char *x);
void pdf_dict_put_string(fz_context *ctx, pdf_obj *dict, pdf_obj *key, const char *x, size_t n);
void pdf_dict_put_text_string(fz_context *ctx, pdf_obj *dict, pdf_obj *key, const char *x);
void pdf_dict_put_rect(fz_context *ctx, pdf_obj *dict, pdf_obj *key, fz_rect x);
void pdf_dict_put_matrix(fz_context *ctx, pdf_obj *dict, pdf_obj *key, fz_matrix x);
void pdf_dict_put_date(fz_context *ctx, pdf_obj *dict, pdf_obj *key, int64_t time);
pdf_obj *pdf_dict_put_array(fz_context *ctx, pdf_obj *dict, pdf_obj *key, int initial);
pdf_obj *pdf_dict_put_dict(fz_context *ctx, pdf_obj *dict, pdf_obj *key, int initial);
pdf_obj *pdf_dict_puts_dict(fz_context *ctx, pdf_obj *dict, const char *key, int initial);

int pdf_dict_get_bool(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
int pdf_dict_get_int(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
int64_t pdf_dict_get_int64(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
float pdf_dict_get_real(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
const char *pdf_dict_get_name(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
const char *pdf_dict_get_string(fz_context *ctx, pdf_obj *dict, pdf_obj *key, size_t *sizep);
const char *pdf_dict_get_text_string(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
fz_rect pdf_dict_get_rect(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
fz_matrix pdf_dict_get_matrix(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
int64_t pdf_dict_get_date(fz_context *ctx, pdf_obj *dict, pdf_obj *key);

int pdf_dict_get_bool_default(fz_context *ctx, pdf_obj *dict, pdf_obj *key, int def);
int pdf_dict_get_int_default(fz_context *ctx, pdf_obj *dict, pdf_obj *key, int def);
float pdf_dict_get_real_default(fz_context *ctx, pdf_obj *dict, pdf_obj *key, float def);

int pdf_dict_get_inheritable_bool(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
int pdf_dict_get_inheritable_int(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
int64_t pdf_dict_get_inheritable_int64(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
float pdf_dict_get_inheritable_real(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
const char *pdf_dict_get_inheritable_name(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
const char *pdf_dict_get_inheritable_string(fz_context *ctx, pdf_obj *dict, pdf_obj *key, size_t *sizep);
const char *pdf_dict_get_inheritable_text_string(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
fz_rect pdf_dict_get_inheritable_rect(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
fz_matrix pdf_dict_get_inheritable_matrix(fz_context *ctx, pdf_obj *dict, pdf_obj *key);
int64_t pdf_dict_get_inheritable_date(fz_context *ctx, pdf_obj *dict, pdf_obj *key);

void pdf_array_push_bool(fz_context *ctx, pdf_obj *array, int x);
void pdf_array_push_int(fz_context *ctx, pdf_obj *array, int64_t x);
void pdf_array_push_real(fz_context *ctx, pdf_obj *array, double x);
void pdf_array_push_name(fz_context *ctx, pdf_obj *array, const char *x);
void pdf_array_push_string(fz_context *ctx, pdf_obj *array, const char *x, size_t n);
void pdf_array_push_text_string(fz_context *ctx, pdf_obj *array, const char *x);
pdf_obj *pdf_array_push_array(fz_context *ctx, pdf_obj *array, int initial);
pdf_obj *pdf_array_push_dict(fz_context *ctx, pdf_obj *array, int initial);

void pdf_array_put_bool(fz_context *ctx, pdf_obj *array, int i, int x);
void pdf_array_put_int(fz_context *ctx, pdf_obj *array, int i, int64_t x);
void pdf_array_put_real(fz_context *ctx, pdf_obj *array, int i, double x);
void pdf_array_put_name(fz_context *ctx, pdf_obj *array, int i, const char *x);
void pdf_array_put_string(fz_context *ctx, pdf_obj *array, int i, const char *x, size_t n);
void pdf_array_put_text_string(fz_context *ctx, pdf_obj *array, int i, const char *x);
pdf_obj *pdf_array_put_array(fz_context *ctx, pdf_obj *array, int i, int initial);
pdf_obj *pdf_array_put_dict(fz_context *ctx, pdf_obj *array, int i, int initial);

int pdf_array_get_bool(fz_context *ctx, pdf_obj *array, int index);
int pdf_array_get_int(fz_context *ctx, pdf_obj *array, int index);
float pdf_array_get_real(fz_context *ctx, pdf_obj *array, int index);
const char *pdf_array_get_name(fz_context *ctx, pdf_obj *array, int index);
const char *pdf_array_get_string(fz_context *ctx, pdf_obj *array, int index, size_t *sizep);
const char *pdf_array_get_text_string(fz_context *ctx, pdf_obj *array, int index);
fz_rect pdf_array_get_rect(fz_context *ctx, pdf_obj *array, int index);
fz_matrix pdf_array_get_matrix(fz_context *ctx, pdf_obj *array, int index);

void pdf_set_obj_parent(fz_context *ctx, pdf_obj *obj, int num);

int pdf_obj_refs(fz_context *ctx, pdf_obj *ref);

int pdf_obj_parent_num(fz_context *ctx, pdf_obj *obj);

char *pdf_sprint_obj(fz_context *ctx, char *buf, size_t cap, size_t *len, pdf_obj *obj, int tight, int ascii);
void pdf_print_obj(fz_context *ctx, fz_output *out, pdf_obj *obj, int tight, int ascii);
void pdf_print_encrypted_obj(fz_context *ctx, fz_output *out, pdf_obj *obj, int tight, int ascii, pdf_crypt *crypt, int num, int gen, int *sep);

void pdf_debug_obj(fz_context *ctx, pdf_obj *obj);
void pdf_debug_ref(fz_context *ctx, pdf_obj *obj);

/*
	Convert Unicode/PdfDocEncoding string into utf-8.

	The returned string must be freed by the caller.
*/
char *pdf_new_utf8_from_pdf_string(fz_context *ctx, const char *srcptr, size_t srclen);

/*
	Convert text string object to UTF-8.

	The returned string must be freed by the caller.
*/
char *pdf_new_utf8_from_pdf_string_obj(fz_context *ctx, pdf_obj *src);

/*
	Load text stream and convert to UTF-8.

	The returned string must be freed by the caller.
*/
char *pdf_new_utf8_from_pdf_stream_obj(fz_context *ctx, pdf_obj *src);

/*
	Load text stream or text string and convert to UTF-8.

	The returned string must be freed by the caller.
*/
char *pdf_load_stream_or_string_as_utf8(fz_context *ctx, pdf_obj *src);

fz_quad pdf_to_quad(fz_context *ctx, pdf_obj *array, int offset);
fz_rect pdf_to_rect(fz_context *ctx, pdf_obj *array);
fz_matrix pdf_to_matrix(fz_context *ctx, pdf_obj *array);
int64_t pdf_to_date(fz_context *ctx, pdf_obj *time);

/*
	pdf_get_indirect_document and pdf_get_bound_document are
	now deprecated. Please do not use them in future. They will
	be removed.

	Please use pdf_pin_document instead.
*/
pdf_document *pdf_get_indirect_document(fz_context *ctx, pdf_obj *obj);
pdf_document *pdf_get_bound_document(fz_context *ctx, pdf_obj *obj);

/*
	pdf_pin_document returns a new reference to the document
	to which obj is bound. The caller is responsible for
	dropping this reference once they have finished with it.

	This is a replacement for pdf_get_indirect_document
	and pdf_get_bound_document that are now deprecated. Those
	returned a borrowed reference that did not need to be
	dropped.

	Note that this can validly return NULL in various cases:
	1) When the object is of a simple type (such as a number
	or a string), it contains no reference to the enclosing
	document. 2) When the object has yet to be inserted into
	a PDF document (such as during parsing). 3) And (in
	future versions) when the document has been destroyed
	but the object reference remains.

	It is the caller's responsibility to deal with a NULL
	return here.
*/
pdf_document *pdf_pin_document(fz_context *ctx, pdf_obj *obj);

void pdf_set_int(fz_context *ctx, pdf_obj *obj, int64_t i);

/* Voodoo to create PDF_NAME(Foo) macros from name-table.h */

#define PDF_NAME(X) ((pdf_obj*)(intptr_t)PDF_ENUM_NAME_##X)

#define PDF_MAKE_NAME(STRING,NAME) PDF_ENUM_NAME_##NAME,
enum {
	PDF_ENUM_NULL,
	PDF_ENUM_TRUE,
	PDF_ENUM_FALSE,
// Copyright (C) 2004-2023 Artifex Software, Inc.
//
// This file is part of MuPDF.
//
// MuPDF is free software: you can redistribute it and/or modify it under the
// terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// MuPDF is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
// details.
//
// You should have received a copy of the GNU Affero General Public License
// along with MuPDF. If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
//
// Alternative licensing terms are available from the licensor.
// For commercial licensing, see <https://www.artifex.com/> or contact
// Artifex Software, Inc., 39 Mesa Street, Suite 108A, San Francisco,
// CA 94129, USA, for further information.

/* Alphabetically sorted list of all PDF names to be available as constants */
PDF_MAKE_NAME("1.2", 1_2)
PDF_MAKE_NAME("1.5", 1_5)
PDF_MAKE_NAME("3D", 3D)
PDF_MAKE_NAME("A", A)
PDF_MAKE_NAME("A85", A85)
PDF_MAKE_NAME("AA", AA)
PDF_MAKE_NAME("AC", AC)
PDF_MAKE_NAME("AESV2", AESV2)
PDF_MAKE_NAME("AESV3", AESV3)
PDF_MAKE_NAME("AHx", AHx)
PDF_MAKE_NAME("AP", AP)
PDF_MAKE_NAME("AS", AS)
PDF_MAKE_NAME("ASCII85Decode", ASCII85Decode)
PDF_MAKE_NAME("ASCIIHexDecode", ASCIIHexDecode)
PDF_MAKE_NAME("AcroForm", AcroForm)
PDF_MAKE_NAME("Action", Action)
PDF_MAKE_NAME("ActualText", ActualText)
PDF_MAKE_NAME("Adobe.PPKLite", Adobe_PPKLite)
PDF_MAKE_NAME("All", All)
PDF_MAKE_NAME("AllOff", AllOff)
PDF_MAKE_NAME("AllOn", AllOn)
PDF_MAKE_NAME("Alpha", Alpha)
PDF_MAKE_NAME("Alt", Alt)
PDF_MAKE_NAME("Alternate", Alternate)
PDF_MAKE_NAME("Annot", Annot)
PDF_MAKE_NAME("Annots", Annots)
PDF_MAKE_NAME("AnyOff", AnyOff)
PDF_MAKE_NAME("App", App)
PDF_MAKE_NAME("Approved", Approved)
PDF_MAKE_NAME("Art", Art)
PDF_MAKE_NAME("ArtBox", ArtBox)
PDF_MAKE_NAME("Artifact", Artifact)
PDF_MAKE_NAME("AsIs", AsIs)
PDF_MAKE_NAME("Ascent", Ascent)
PDF_MAKE_NAME("Aside", Aside)
PDF_MAKE_NAME("AuthEvent", AuthEvent)
PDF_MAKE_NAME("Author", Author)
PDF_MAKE_NAME("B", B)
PDF_MAKE_NAME("BBox", BBox)
PDF_MAKE_NAME("BC", BC)
PDF_MAKE_NAME("BE", BE)
PDF_MAKE_NAME("BG", BG)
PDF_MAKE_NAME("BM", BM)
PDF_MAKE_NAME("BPC", BPC)
PDF_MAKE_NAME("BS", BS)
PDF_MAKE_NAME("Background", Background)
PDF_MAKE_NAME("BaseEncoding", BaseEncoding)
PDF_MAKE_NAME("BaseFont", BaseFont)
PDF_MAKE_NAME("BaseState", BaseState)
PDF_MAKE_NAME("BibEntry", BibEntry)
PDF_MAKE_NAME("BitsPerComponent", BitsPerComponent)
PDF_MAKE_NAME("BitsPerCoordinate", BitsPerCoordinate)
PDF_MAKE_NAME("BitsPerFlag", BitsPerFlag)
PDF_MAKE_NAME("BitsPerSample", BitsPerSample)
PDF_MAKE_NAME("BlackIs1", BlackIs1)
PDF_MAKE_NAME("BlackPoint", BlackPoint)
PDF_MAKE_NAME("BleedBox", BleedBox)
PDF_MAKE_NAME("Blinds", Blinds)
PDF_MAKE_NAME("BlockQuote", BlockQuote)
PDF_MAKE_NAME("Border", Border)
PDF_MAKE_NAME("Bounds", Bounds)
PDF_MAKE_NAME("Box", Box)
PDF_MAKE_NAME("Bt", Bt)
PDF_MAKE_NAME("Btn", Btn)
PDF_MAKE_NAME("Butt", Butt)
PDF_MAKE_NAME("ByteRange", ByteRange)
PDF_MAKE_NAME("C", C)
PDF_MAKE_NAME("C0", C0)
PDF_MAKE_NAME("C1", C1)
PDF_MAKE_NAME("CA", CA)
PDF_MAKE_NAME("CCF", CCF)
PDF_MAKE_NAME("CCITTFaxDecode", CCITTFaxDecode)
PDF_MAKE_NAME("CF", CF)
PDF_MAKE_NAME("CFM", CFM)
PDF_MAKE_NAME("CI", CI)
PDF_MAKE_NAME("CIDFontType0", CIDFontType0)
PDF_MAKE_NAME("CIDFontType0C", CIDFontType0C)
PDF_MAKE_NAME("CIDFontType2", CIDFontType2)
PDF_MAKE_NAME("CIDSystemInfo", CIDSystemInfo)
PDF_MAKE_NAME("CIDToGIDMap", CIDToGIDMap)
PDF_MAKE_NAME("CMYK", CMYK)
PDF_MAKE_NAME("CS", CS)
PDF_MAKE_NAME("CalCMYK", CalCMYK)
PDF_MAKE_NAME("CalGray", CalGray)
PDF_MAKE_NAME("CalRGB", CalRGB)
PDF_MAKE_NAME("CapHeight", CapHeight)
PDF_MAKE_NAME("Caption", Caption)
PDF_MAKE_NAME("Caret", Caret)
PDF_MAKE_NAME("Catalog", Catalog)
PDF_MAKE_NAME("Cert", Cert)
PDF_MAKE_NAME("Ch", Ch)
PDF_MAKE_NAME("Changes", Changes)
PDF_MAKE_NAME("CharProcs", CharProcs)
PDF_MAKE_NAME("CheckSum", CheckSum)
PDF_MAKE_NAME("Circle", Circle)
PDF_MAKE_NAME("ClosedArrow", ClosedArrow)
PDF_MAKE_NAME("Code", Code)
PDF_MAKE_NAME("Collection", Collection)
PDF_MAKE_NAME("ColorSpace", ColorSpace)
PDF_MAKE_NAME("ColorTransform", ColorTransform)
PDF_MAKE_NAME("Colorants", Colorants)
PDF_MAKE_NAME("Colors", Colors)
PDF_MAKE_NAME("Columns", Columns)
PDF_MAKE_NAME("Confidential", Confidential)
PDF_MAKE_NAME("Configs", Configs)
PDF_MAKE_NAME("ContactInfo", ContactInfo)
PDF_MAKE_NAME("Contents", Contents)
PDF_MAKE_NAME("Coords", Coords)
PDF_MAKE_NAME("Count", Count)
PDF_MAKE_NAME("Cover", Cover)
PDF_MAKE_NAME("CreationDate", CreationDate)
PDF_MAKE_NAME("Creator", Creator)
PDF_MAKE_NAME("CropBox", CropBox)
PDF_MAKE_NAME("Crypt", Crypt)
PDF_MAKE_NAME("D", D)
PDF_MAKE_NAME("DA", DA)
PDF_MAKE_NAME("DC", DC)
PDF_MAKE_NAME("DCT", DCT)
PDF_MAKE_NAME("DCTDecode", DCTDecode)
PDF_MAKE_NAME("DL", DL)
PDF_MAKE_NAME("DOS", DOS)
PDF_MAKE_NAME("DP", DP)
PDF_MAKE_NAME("DR", DR)
PDF_MAKE_NAME("DS", DS)
PDF_MAKE_NAME("DV", DV)
PDF_MAKE_NAME("DW", DW)
PDF_MAKE_NAME("DW2", DW2)
PDF_MAKE_NAME("DamagedRowsBeforeError", DamagedRowsBeforeError)
PDF_MAKE_NAME("Data", Data)
PDF_MAKE_NAME("Date", Date)
PDF_MAKE_NAME("Decode", Decode)
PDF_MAKE_NAME("DecodeParms", DecodeParms)
PDF_MAKE_NAME("Default", Default)
PDF_MAKE_NAME("DefaultCMYK", DefaultCMYK)
PDF_MAKE_NAME("DefaultGray", DefaultGray)
PDF_MAKE_NAME("DefaultRGB", DefaultRGB)
PDF_MAKE_NAME("Departmental", Departmental)
PDF_MAKE_NAME("Desc", Desc)
PDF_MAKE_NAME("DescendantFonts", DescendantFonts)
PDF_MAKE_NAME("Descent", Descent)
PDF_MAKE_NAME("Design", Design)
PDF_MAKE_NAME("Dest", Dest)
PDF_MAKE_NAME("DestOutputProfile", DestOutputProfile)
PDF_MAKE_NAME("Dests", Dests)
PDF_MAKE_NAME("DeviceCMYK", DeviceCMYK)
PDF_MAKE_NAME("DeviceGray", DeviceGray)
PDF_MAKE_NAME("DeviceN", DeviceN)
PDF_MAKE_NAME("DeviceRGB", DeviceRGB)
PDF_MAKE_NAME("Di", Di)
PDF_MAKE_NAME("Diamond", Diamond)
PDF_MAKE_NAME("Differences", Differences)
PDF_MAKE_NAME("DigestLocation", DigestLocation)
PDF_MAKE_NAME("DigestMethod", DigestMethod)
PDF_MAKE_NAME("DigestValue", DigestValue)
PDF_MAKE_NAME("Dissolve", Dissolve)
PDF_MAKE_NAME("Div", Div)
PDF_MAKE_NAME("Dm", Dm)
PDF_MAKE_NAME("DocMDP", DocMDP)
PDF_MAKE_NAME("Document", Document)
PDF_MAKE_NAME("DocumentFragment", DocumentFragment)
PDF_MAKE_NAME("Domain", Domain)
PDF_MAKE_NAME("Draft", Draft)
PDF_MAKE_NAME("Dur", Dur)
PDF_MAKE_NAME("E", E)
PDF_MAKE_NAME("EF", EF)
PDF_MAKE_NAME("EarlyChange", EarlyChange)
PDF_MAKE_NAME("Em", Em)
PDF_MAKE_NAME("EmbeddedFile", EmbeddedFile)
PDF_MAKE_NAME("EmbeddedFiles", EmbeddedFiles)
PDF_MAKE_NAME("Encode", Encode)
PDF_MAKE_NAME("EncodedByteAlign", EncodedByteAlign)
PDF_MAKE_NAME("Encoding", Encoding)
PDF_MAKE_NAME("Encrypt", Encrypt)
PDF_MAKE_NAME("EncryptMetadata", EncryptMetadata)
PDF_MAKE_NAME("EndOfBlock", EndOfBlock)
PDF_MAKE_NAME("EndOfLine", EndOfLine)
PDF_MAKE_NAME("Exclude", Exclude)
PDF_MAKE_NAME("Experimental", Experimental)
PDF_MAKE_NAME("Expired", Expired)
PDF_MAKE_NAME("ExtGState", ExtGState)
PDF_MAKE_NAME("Extend", Extend)
PDF_MAKE_NAME("F", F)
PDF_MAKE_NAME("FENote", FENote)
PDF_MAKE_NAME("FL", FL)
PDF_MAKE_NAME("FRM", FRM)
PDF_MAKE_NAME("FS", FS)
PDF_MAKE_NAME("FT", FT)
PDF_MAKE_NAME("Fade", Fade)
PDF_MAKE_NAME("Ff", Ff)
PDF_MAKE_NAME("FieldMDP", FieldMDP)
PDF_MAKE_NAME("Fields", Fields)
PDF_MAKE_NAME("Figure", Figure)
PDF_MAKE_NAME("FileAttachment", FileAttachment)
PDF_MAKE_NAME("FileSize", FileSize)
PDF_MAKE_NAME("Filespec", Filespec)
PDF_MAKE_NAME("Filter", Filter)
PDF_MAKE_NAME("Final", Final)
PDF_MAKE_NAME("Fingerprint", Fingerprint)
PDF_MAKE_NAME("First", First)
PDF_MAKE_NAME("FirstChar", FirstChar)
PDF_MAKE_NAME("FirstPage", FirstPage)
PDF_MAKE_NAME("Fit", Fit)
PDF_MAKE_NAME("FitB", FitB)
PDF_MAKE_NAME("FitBH", FitBH)
PDF_MAKE_NAME("FitBV", FitBV)
PDF_MAKE_NAME("FitH", FitH)
PDF_MAKE_NAME("FitR", FitR)
PDF_MAKE_NAME("FitV", FitV)
PDF_MAKE_NAME("Fl", Fl)
PDF_MAKE_NAME("Flags", Flags)
PDF_MAKE_NAME("FlateDecode", FlateDecode)
PDF_MAKE_NAME("Fly", Fly)
PDF_MAKE_NAME("Font", Font)
PDF_MAKE_NAME("FontBBox", FontBBox)
PDF_MAKE_NAME("FontDescriptor", FontDescriptor)
PDF_MAKE_NAME("FontFile", FontFile)
PDF_MAKE_NAME("FontFile2", FontFile2)
PDF_MAKE_NAME("FontFile3", FontFile3)
PDF_MAKE_NAME("FontMatrix", FontMatrix)
PDF_MAKE_NAME("FontName", FontName)
PDF_MAKE_NAME("ForComment", ForComment)
PDF_MAKE_NAME("ForPublicRelease", ForPublicRelease)
PDF_MAKE_NAME("Form", Form)
PDF_MAKE_NAME("FormEx", FormEx)
PDF_MAKE_NAME("FormType", FormType)
PDF_MAKE_NAME("Formula", Formula)
PDF_MAKE_NAME("FreeText", FreeText)
PDF_MAKE_NAME("FreeTextCallout", FreeTextCallout)
PDF_MAKE_NAME("FreeTextTypeWriter", FreeTextTypeWriter)
PDF_MAKE_NAME("Function", Function)
PDF_MAKE_NAME("FunctionType", FunctionType)
PDF_MAKE_NAME("Functions", Functions)
PDF_MAKE_NAME("G", G)
PDF_MAKE_NAME("GTS_PDFX", GTS_PDFX)
PDF_MAKE_NAME("Gamma", Gamma)
PDF_MAKE_NAME("Glitter", Glitter)
PDF_MAKE_NAME("GoTo", GoTo)
PDF_MAKE_NAME("GoToR", GoToR)
PDF_MAKE_NAME("Group", Group)
PDF_MAKE_NAME("H", H)
PDF_MAKE_NAME("H1", H1)
PDF_MAKE_NAME("H2", H2)
PDF_MAKE_NAME("H3", H3)
PDF_MAKE_NAME("H4", H4)
PDF_MAKE_NAME("H5", H5)
PDF_MAKE_NAME("H6", H6)
PDF_MAKE_NAME("Height", Height)
PDF_MAKE_NAME("Helv", Helv)
PDF_MAKE_NAME("Highlight", Highlight)
PDF_MAKE_NAME("HistoryPos", HistoryPos)
PDF_MAKE_NAME("I", I)
PDF_MAKE_NAME("IC", IC)
PDF_MAKE_NAME("ICCBased", ICCBased)
PDF_MAKE_NAME("ID", ID)
PDF_MAKE_NAME("IM", IM)
PDF_MAKE_NAME("IRT", IRT)
PDF_MAKE_NAME("IT", IT)
PDF_MAKE_NAME("Identity", Identity)
PDF_MAKE_NAME("Identity-H", Identity_H)
PDF_MAKE_NAME("Identity-V", Identity_V)
PDF_MAKE_NAME("Image", Image)
PDF_MAKE_NAME("ImageB", ImageB)
PDF_MAKE_NAME("ImageC", ImageC)
PDF_MAKE_NAME("ImageI", ImageI)
PDF_MAKE_NAME("ImageMask", ImageMask)
PDF_MAKE_NAME("Include", Include)
PDF_MAKE_NAME("Index", Index)
PDF_MAKE_NAME("Indexed", Indexed)
PDF_MAKE_NAME("Info", Info)
PDF_MAKE_NAME("Ink", Ink)
PDF_MAKE_NAME("InkList", InkList)
PDF_MAKE_NAME("Intent", Intent)
PDF_MAKE_NAME("Interpolate", Interpolate)
PDF_MAKE_NAME("IsMap", IsMap)
PDF_MAKE_NAME("ItalicAngle", ItalicAngle)
PDF_MAKE_NAME("JBIG2Decode", JBIG2Decode)
PDF_MAKE_NAME("JBIG2Globals", JBIG2Globals)
PDF_MAKE_NAME("JPXDecode", JPXDecode)
PDF_MAKE_NAME("JS", JS)
PDF_MAKE_NAME("JavaScript", JavaScript)
PDF_MAKE_NAME("K", K)
PDF_MAKE_NAME("Keywords", Keywords)
PDF_MAKE_NAME("Kids", Kids)
PDF_MAKE_NAME("L", L)
PDF_MAKE_NAME("LBody", LBody)
PDF_MAKE_NAME("LC", LC)
PDF_MAKE_NAME("LE", LE)
PDF_MAKE_NAME("LI", LI)
PDF_MAKE_NAME("LJ", LJ)
PDF_MAKE_NAME("LW", LW)
PDF_MAKE_NAME("LZ", LZ)
PDF_MAKE_NAME("LZW", LZW)
PDF_MAKE_NAME("LZWDecode", LZWDecode)
PDF_MAKE_NAME("Lab", Lab)
PDF_MAKE_NAME("Label", Label)
PDF_MAKE_NAME("Lang", Lang)
PDF_MAKE_NAME("Last", Last)
PDF_MAKE_NAME("LastChar", LastChar)
PDF_MAKE_NAME("LastPage", LastPage)
PDF_MAKE_NAME("Launch", Launch)
PDF_MAKE_NAME("Layer", Layer)
PDF_MAKE_NAME("Lbl", Lbl)
PDF_MAKE_NAME("Length", Length)
PDF_MAKE_NAME("Length1", Length1)
PDF_MAKE_NAME("Length2", Length2)
PDF_MAKE_NAME("Length3", Length3)
PDF_MAKE_NAME("Limits", Limits)
PDF_MAKE_NAME("Line", Line)
PDF_MAKE_NAME("LineArrow", LineArrow)
PDF_MAKE_NAME("LineDimension", LineDimension)
PDF_MAKE_NAME("Linearized", Linearized)
PDF_MAKE_NAME("Link", Link)
PDF_MAKE_NAME("List", List)
PDF_MAKE_NAME("Location", Location)
PDF_MAKE_NAME("Lock", Lock)
PDF_MAKE_NAME("Locked", Locked)
PDF_MAKE_NAME("Luminosity", Luminosity)
PDF_MAKE_NAME("M", M)
PDF_MAKE_NAME("MCID", MCID)
PDF_MAKE_NAME("MK", MK)
PDF_MAKE_NAME("ML", ML)
PDF_MAKE_NAME("MMType1", MMType1)
PDF_MAKE_NAME("Mac", Mac)
PDF_MAKE_NAME("Mask", Mask)
PDF_MAKE_NAME("Matrix", Matrix)
PDF_MAKE_NAME("Matte", Matte)
PDF_MAKE_NAME("MaxLen", MaxLen)
PDF_MAKE_NAME("MediaBox", MediaBox)
PDF_MAKE_NAME("Metadata", Metadata)
PDF_MAKE_NAME("MissingWidth", MissingWidth)
PDF_MAKE_NAME("ModDate", ModDate)
PDF_MAKE_NAME("Movie", Movie)
PDF_MAKE_NAME("Msg", Msg)
PDF_MAKE_NAME("Multiply", Multiply)
PDF_MAKE_NAME("N", N)
PDF_MAKE_NAME("Name", Name)
PDF_MAKE_NAME("Named", Named)
PDF_MAKE_NAME("Names", Names)
PDF_MAKE_NAME("NewWindow", NewWindow)
PDF_MAKE_NAME("Next", Next)
PDF_MAKE_NAME("NextPage", NextPage)
PDF_MAKE_NAME("NonEFontNoWarn", NonEFontNoWarn)
PDF_MAKE_NAME("NonStruct", NonStruct)
PDF_MAKE_NAME("None", None)
PDF_MAKE_NAME("Normal", Normal)
PDF_MAKE_NAME("NotApproved", NotApproved)
PDF_MAKE_NAME("NotForPublicRelease", NotForPublicRelease)
PDF_MAKE_NAME("Note", Note)
PDF_MAKE_NAME("NumSections", NumSections)
PDF_MAKE_NAME("Nums", Nums)
PDF_MAKE_NAME("O", O)
PDF_MAKE_NAME("OC", OC)
PDF_MAKE_NAME("OCG", OCG)
PDF_MAKE_NAME("OCGs", OCGs)
PDF_MAKE_NAME("OCMD", OCMD)
PDF_MAKE_NAME("OCProperties", OCProperties)
PDF_MAKE_NAME("OE", OE)
PDF_MAKE_NAME("OFF", OFF)
PDF_MAKE_NAME("ON", ON)
PDF_MAKE_NAME("OP", OP)
PDF_MAKE_NAME("OPM", OPM)
PDF_MAKE_NAME("OS", OS)
PDF_MAKE_NAME("ObjStm", ObjStm)
PDF_MAKE_NAME("Of", Of)
PDF_MAKE_NAME("Off", Off)
PDF_MAKE_NAME("Open", Open)
PDF_MAKE_NAME("OpenArrow", OpenArrow)
PDF_MAKE_NAME("OpenType", OpenType)
PDF_MAKE_NAME("Opt", Opt)
PDF_MAKE_NAME("Order", Order)
PDF_MAKE_NAME("Ordering", Ordering)
PDF_MAKE_NAME("Outlines", Outlines)
PDF_MAKE_NAME("OutputCondition", OutputCondition)
PDF_MAKE_NAME("OutputConditionIdentifier", OutputConditionIdentifier)
PDF_MAKE_NAME("OutputIntent", OutputIntent)
PDF_MAKE_NAME("OutputIntents", OutputIntents)
PDF_MAKE_NAME("P", P)
PDF_MAKE_NAME("PDF", PDF)
PDF_MAKE_NAME("PS", PS)
PDF_MAKE_NAME("Page", Page)
PDF_MAKE_NAME("PageLabels", PageLabels)
PDF_MAKE_NAME("PageMode", PageMode)
PDF_MAKE_NAME("Pages", Pages)
PDF_MAKE_NAME("PaintType", PaintType)
PDF_MAKE_NAME("Params", Params)
PDF_MAKE_NAME("Parent", Parent)
PDF_MAKE_NAME("ParentTree", ParentTree)
PDF_MAKE_NAME("Part", Part)
PDF_MAKE_NAME("Pattern", Pattern)
PDF_MAKE_NAME("PatternType", PatternType)
PDF_MAKE_NAME("Perms", Perms)
PDF_MAKE_NAME("PolyLine", PolyLine)
PDF_MAKE_NAME("PolyLineDimension", PolyLineDimension)
PDF_MAKE_NAME("Polygon", Polygon)
PDF_MAKE_NAME("PolygonCloud", PolygonCloud)
PDF_MAKE_NAME("PolygonDimension", PolygonDimension)
PDF_MAKE_NAME("Popup", Popup)
PDF_MAKE_NAME("PreRelease", PreRelease)
PDF_MAKE_NAME("Predictor", Predictor)
PDF_MAKE_NAME("Prev", Prev)
PDF_MAKE_NAME("PrevPage", PrevPage)
PDF_MAKE_NAME("Preview", Preview)
PDF_MAKE_NAME("Print", Print)
PDF_MAKE_NAME("PrinterMark", PrinterMark)
PDF_MAKE_NAME("Private", Private)
PDF_MAKE_NAME("ProcSet", ProcSet)
PDF_MAKE_NAME("Producer", Producer)
PDF_MAKE_NAME("Prop_AuthTime", Prop_AuthTime)
PDF_MAKE_NAME("Prop_AuthType", Prop_AuthType)
PDF_MAKE_NAME("Prop_Build", Prop_Build)
PDF_MAKE_NAME("Properties", Properties)
PDF_MAKE_NAME("PubSec", PubSec)
PDF_MAKE_NAME("Push", Push)
PDF_MAKE_NAME("Q", Q)
PDF_MAKE_NAME("QuadPoints", QuadPoints)
PDF_MAKE_NAME("Quote", Quote)
PDF_MAKE_NAME("R", R)
PDF_MAKE_NAME("RB", RB)
PDF_MAKE_NAME("RBGroups", RBGroups)
PDF_MAKE_NAME("RC", RC)
PDF_MAKE_NAME("RClosedArrow", RClosedArrow)
PDF_MAKE_NAME("RD", RD)
PDF_MAKE_NAME("REx", REx)
PDF_MAKE_NAME("RGB", RGB)
PDF_MAKE_NAME("RI", RI)
PDF_MAKE_NAME("RL", RL)
PDF_MAKE_NAME("ROpenArrow", ROpenArrow)
PDF_MAKE_NAME("RP", RP)
PDF_MAKE_NAME("RT", RT)
PDF_MAKE_NAME("Range", Range)
PDF_MAKE_NAME("Reason", Reason)
PDF_MAKE_NAME("Rect", Rect)
PDF_MAKE_NAME("Redact", Redact)
PDF_MAKE_NAME("Ref", Ref)
PDF_MAKE_NAME("Reference", Reference)
PDF_MAKE_NAME("Registry", Registry)
PDF_MAKE_NAME("ResetForm", ResetForm)
PDF_MAKE_NAME("Resources", Resources)
PDF_MAKE_NAME("RoleMap", RoleMap)
PDF_MAKE_NAME("Root", Root)
PDF_MAKE_NAME("Rotate", Rotate)
PDF_MAKE_NAME("Rows", Rows)
PDF_MAKE_NAME("Ruby", Ruby)
PDF_MAKE_NAME("RunLengthDecode", RunLengthDecode)
PDF_MAKE_NAME("S", S)
PDF_MAKE_NAME("SMask", SMask)
PDF_MAKE_NAME("SMaskInData", SMaskInData)
PDF_MAKE_NAME("Schema", Schema)
PDF_MAKE_NAME("Screen", Screen)
PDF_MAKE_NAME("Sect", Sect)
PDF_MAKE_NAME("Separation", Separation)
PDF_MAKE_NAME("Shading", Shading)
PDF_MAKE_NAME("ShadingType", ShadingType)
PDF_MAKE_NAME("Si", Si)
PDF_MAKE_NAME("Sig", Sig)
PDF_MAKE_NAME("SigFlags", SigFlags)
PDF_MAKE_NAME("SigQ", SigQ)
PDF_MAKE_NAME("SigRef", SigRef)
PDF_MAKE_NAME("Size", Size)
PDF_MAKE_NAME("Slash", Slash)
PDF_MAKE_NAME("Sold", Sold)
PDF_MAKE_NAME("Sound", Sound)
PDF_MAKE_NAME("Span", Span)
PDF_MAKE_NAME("Split", Split)
PDF_MAKE_NAME("Square", Square)
PDF_MAKE_NAME("Squiggly", Squiggly)
PDF_MAKE_NAME("St", St)
PDF_MAKE_NAME("Stamp", Stamp)
PDF_MAKE_NAME("StampImage", StampImage)
PDF_MAKE_NAME("StampSnapshot", StampSnapshot)
PDF_MAKE_NAME("Standard", Standard)
PDF_MAKE_NAME("StdCF", StdCF)
PDF_MAKE_NAME("StemV", StemV)
PDF_MAKE_NAME("StmF", StmF)
PDF_MAKE_NAME("StrF", StrF)
PDF_MAKE_NAME("StrikeOut", StrikeOut)
PDF_MAKE_NAME("Strong", Strong)
PDF_MAKE_NAME("StructParent", StructParent)
PDF_MAKE_NAME("StructParents", StructParents)
PDF_MAKE_NAME("StructTreeRoot", StructTreeRoot)
PDF_MAKE_NAME("Sub", Sub)
PDF_MAKE_NAME("SubFilter", SubFilter)
PDF_MAKE_NAME("Subject", Subject)
PDF_MAKE_NAME("Subtype", Subtype)
PDF_MAKE_NAME("Subtype2", Subtype2)
PDF_MAKE_NAME("Supplement", Supplement)
PDF_MAKE_NAME("Symb", Symb)
PDF_MAKE_NAME("T", T)
PDF_MAKE_NAME("TBody", TBody)
PDF_MAKE_NAME("TD", TD)
PDF_MAKE_NAME("TFoot", TFoot)
PDF_MAKE_NAME("TH", TH)
PDF_MAKE_NAME("THead", THead)
PDF_MAKE_NAME("TI", TI)
PDF_MAKE_NAME("TOC", TOC)
PDF_MAKE_NAME("TOCI", TOCI)
PDF_MAKE_NAME("TR", TR)
PDF_MAKE_NAME("TR2", TR2)
PDF_MAKE_NAME("TU", TU)
PDF_MAKE_NAME("Table", Table)
PDF_MAKE_NAME("Text", Text)
PDF_MAKE_NAME("TilingType", TilingType)
PDF_MAKE_NAME("Times", Times)
PDF_MAKE_NAME("Title", Title)
PDF_MAKE_NAME("ToUnicode", ToUnicode)
PDF_MAKE_NAME("TopSecret", TopSecret)
PDF_MAKE_NAME("Trans", Trans)
PDF_MAKE_NAME("TransformMethod", TransformMethod)
PDF_MAKE_NAME("TransformParams", TransformParams)
PDF_MAKE_NAME("Transparency", Transparency)
PDF_MAKE_NAME("TrapNet", TrapNet)
PDF_MAKE_NAME("TrimBox", TrimBox)
PDF_MAKE_NAME("TrueType", TrueType)
PDF_MAKE_NAME("TrustedMode", TrustedMode)
PDF_MAKE_NAME("Tx", Tx)
PDF_MAKE_NAME("Type", Type)
PDF_MAKE_NAME("Type0", Type0)
PDF_MAKE_NAME("Type1", Type1)
PDF_MAKE_NAME("Type1C", Type1C)
PDF_MAKE_NAME("Type3", Type3)
PDF_MAKE_NAME("U", U)
PDF_MAKE_NAME("UE", UE)
PDF_MAKE_NAME("UF", UF)
PDF_MAKE_NAME("URI", URI)
PDF_MAKE_NAME("URL", URL)
PDF_MAKE_NAME("Unchanged", Unchanged)
PDF_MAKE_NAME("Uncover", Uncover)
PDF_MAKE_NAME("Underline", Underline)
PDF_MAKE_NAME("Unix", Unix)
PDF_MAKE_NAME("Usage", Usage)
PDF_MAKE_NAME("UseBlackPtComp", UseBlackPtComp)
PDF_MAKE_NAME("UseCMap", UseCMap)
PDF_MAKE_NAME("UseOutlines", UseOutlines)
PDF_MAKE_NAME("UserUnit", UserUnit)
PDF_MAKE_NAME("V", V)
PDF_MAKE_NAME("V2", V2)
PDF_MAKE_NAME("VE", VE)
PDF_MAKE_NAME("Version", Version)
PDF_MAKE_NAME("Vertices", Vertices)
PDF_MAKE_NAME("VerticesPerRow", VerticesPerRow)
PDF_MAKE_NAME("View", View)
PDF_MAKE_NAME("W", W)
PDF_MAKE_NAME("W2", W2)
PDF_MAKE_NAME("WMode", WMode)
PDF_MAKE_NAME("WP", WP)
PDF_MAKE_NAME("WT", WT)
PDF_MAKE_NAME("Warichu", Warichu)
PDF_MAKE_NAME("Watermark", Watermark)
PDF_MAKE_NAME("WhitePoint", WhitePoint)
PDF_MAKE_NAME("Widget", Widget)
PDF_MAKE_NAME("Width", Width)
PDF_MAKE_NAME("Widths", Widths)
PDF_MAKE_NAME("WinAnsiEncoding", WinAnsiEncoding)
PDF_MAKE_NAME("Wipe", Wipe)
PDF_MAKE_NAME("XFA", XFA)
PDF_MAKE_NAME("XHeight", XHeight)
PDF_MAKE_NAME("XML", XML)
PDF_MAKE_NAME("XObject", XObject)
PDF_MAKE_NAME("XRef", XRef)
PDF_MAKE_NAME("XRefStm", XRefStm)
PDF_MAKE_NAME("XStep", XStep)
PDF_MAKE_NAME("XYZ", XYZ)
PDF_MAKE_NAME("YStep", YStep)
PDF_MAKE_NAME("Yes", Yes)
PDF_MAKE_NAME("ZaDb", ZaDb)
PDF_MAKE_NAME("a", a)
PDF_MAKE_NAME("adbe.pkcs7.detached", adbe_pkcs7_detached)
PDF_MAKE_NAME("ca", ca)
PDF_MAKE_NAME("n0", n0)
PDF_MAKE_NAME("n1", n1)
PDF_MAKE_NAME("n2", n2)
PDF_MAKE_NAME("op", op)
PDF_MAKE_NAME("r", r)
	PDF_ENUM_LIMIT,
};
#undef PDF_MAKE_NAME

#define PDF_NULL ((pdf_obj*)(intptr_t)PDF_ENUM_NULL)
#define PDF_TRUE ((pdf_obj*)(intptr_t)PDF_ENUM_TRUE)
#define PDF_FALSE ((pdf_obj*)(intptr_t)PDF_ENUM_FALSE)
#define PDF_LIMIT ((pdf_obj*)(intptr_t)PDF_ENUM_LIMIT)


/* Implementation details: subject to change. */

/*
	for use by pdf_crypt_obj_imp to decrypt AES string in place
*/
void pdf_set_str_len(fz_context *ctx, pdf_obj *obj, size_t newlen);


/* Journalling */

/* Call this to enable journalling on a given document. */
void pdf_enable_journal(fz_context *ctx, pdf_document *doc);

/* Call this to start an operation. Undo/redo works at 'operation'
 * granularity. Nested operations are all counted within the outermost
 * operation. Any modification performed on a journalled PDF without an
 * operation having been started will throw an error. */
void pdf_begin_operation(fz_context *ctx, pdf_document *doc, const char *operation);

/* Call this to start an implicit operation. Implicit operations are
 * operations that happen as a consequence of things like updating
 * an annotation. They get rolled into the previous operation, because
 * they generally happen as a result of them. */
void pdf_begin_implicit_operation(fz_context *ctx, pdf_document *doc);

/* Call this to end an operation. */
void pdf_end_operation(fz_context *ctx, pdf_document *doc);

/* Call this to abandon an operation. Revert to the state
 * when you began. */
void pdf_abandon_operation(fz_context *ctx, pdf_document *doc);

/* Call this to find out how many undo/redo steps there are, and the
 * current position we are within those. 0 = original document,
 * *steps = final edited version. */
int pdf_undoredo_state(fz_context *ctx, pdf_document *doc, int *steps);

/* Call this to find the title of the operation within the undo state. */
const char *pdf_undoredo_step(fz_context *ctx, pdf_document *doc, int step);

/* Helper functions to identify if we are in a state to be able to undo
 * or redo. */
int pdf_can_undo(fz_context *ctx, pdf_document *doc);
int pdf_can_redo(fz_context *ctx, pdf_document *doc);

/* Move backwards in the undo history. Throws an error if we are at the
 * start. Any edits to the document at this point will discard all
 * subsequent history. */
void pdf_undo(fz_context *ctx, pdf_document *doc);

/* Move forwards in the undo history. Throws an error if we are at the
 * end. */
void pdf_redo(fz_context *ctx, pdf_document *doc);

/* Called to reset the entire history. This is called implicitly when
 * a non-undoable change occurs (such as a pdf repair). */
void pdf_discard_journal(fz_context *ctx, pdf_journal *journal);

/* Internal destructor. */
void pdf_drop_journal(fz_context *ctx, pdf_journal *journal);

/* Internal call as part of saving a snapshot of a PDF document. */
void pdf_serialise_journal(fz_context *ctx, pdf_document *doc, fz_output *out);

/* Internal call as part of loading a snapshot of a PDF document. */
void pdf_deserialise_journal(fz_context *ctx, pdf_document *doc, fz_stream *stm);

/* Internal call as part of creating objects. */
void pdf_add_journal_fragment(fz_context *ctx, pdf_document *doc, int parent, pdf_obj *copy, fz_buffer *copy_stream, int newobj);

char *pdf_format_date(fz_context *ctx, int64_t time, char *s, size_t n);
int64_t pdf_parse_date(fz_context *ctx, const char *s);

#endif
