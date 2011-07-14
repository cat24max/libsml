// Copyright 2011 Juri Glass, Mathias Runge, Nadim El Sayed 
// DAI-Labor, TU-Berlin
// 
// This file is part of libSML.
// 
// libSML is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// libSML is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with libSML.  If not, see <http://www.gnu.org/licenses/>.


#include <sml/sml_value.h>

#include <stdio.h>

sml_value *sml_value_parse(sml_buffer *buf) {
	int max = 1;
	int type = sml_buf_get_next_type(buf);
	unsigned char byte = sml_buf_get_current_byte(buf);
	
	sml_value *value = (sml_value *) malloc(sizeof(sml_value));
	value->type = type;

	switch (type) {
		case SML_TYPE_OCTET_STRING:
			value->data.bytes = sml_octet_string_parse(buf);
			break;
		case SML_TYPE_BOOLEAN:
			value->data.boolean = sml_boolean_parse(buf);
			break;
		case SML_TYPE_UNSIGNED:
		case SML_TYPE_INTEGER:
			// get maximal size, if not all bytes are used (example: only 6 bytes for a u64)
			while (max < ((byte & SML_LENGTH_FIELD) - 1)) { 
				max <<= 1;
			}
			
			value->data.uint8 = sml_number_parse(buf, type, max);
			value->type |= max;
			break;
		default:
			buf->error = 1;
			break;
	}
	if (sml_buf_has_errors(buf)) {
		sml_value_free(value);
		return 0;
	}
	
	return value;
}

void sml_value_write(sml_value *value, sml_buffer *buf) {
	if (value == 0) {
		sml_buf_optional_write(buf);
		return;
	}
	
	switch (value->type & SML_TYPE_FIELD) {
		case SML_TYPE_OCTET_STRING:
			sml_octet_string_write(value->data.bytes, buf);
			break;
		case SML_TYPE_BOOLEAN:
			sml_boolean_write(value->data.boolean, buf);
			break;
		case SML_TYPE_UNSIGNED:
		case SML_TYPE_INTEGER:
			sml_number_write(value->data.uint8, (value->type & SML_TYPE_FIELD), 
				(value->type & SML_LENGTH_FIELD), buf);
			break;
	}
}

sml_value *sml_value_init() {
	sml_value *value = (sml_value *) malloc(sizeof(sml_value));
	memset(value, 0, sizeof(value));
	return value;
}

void sml_value_free(sml_value *value) {
	if (value) {
		switch (value->type) {
			case SML_TYPE_OCTET_STRING:
				sml_octet_string_free(value->data.bytes);
				break;
			case SML_TYPE_BOOLEAN:
				sml_boolean_free(value->data.boolean);
				break;
			default:
				sml_number_free(value->data.int8);
				break;
		}
		free(value);
	}
}

