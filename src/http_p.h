#ifndef sKfWRTuG___9H8Dcb_vfV_0___tcQl_u
#define sKfWRTuG___9H8Dcb_vfV_0___tcQl_u 1

#include <stdint.h>

/* This parser is NOT RFC-compliant. It does not support:
1. Multiple spaces, including text spreading across multiple lines
2. Comments in headers
3. Weird things that RFC says are legal, for instance version number being
    greater than a single digit
4. Partial lack of CRLF and other character validation (\r\n could be anything else in some places)
5. Chunk extensions
6. Trailers
And more.

It does support:
1. Chunked transfer
2. DEFLATE and Brotli compression and decompression
3. Stream parsing
4. Parsing only part of a message and resuming parsing
5. Various parsing settings
*/

enum http_p_sconsts {
  /* ERROR CODES */
  http_success,
  http_failure,
  
  /* PROTOCOL */
  http_p_http1 = 0,
  http_p_http2,
  
  /* TRANSFER */
  http_t_none = 0,
  http_t_chunked,
  
  /* ENCODING */
  http_e_none = 0,
  http_e_brotli,
  http_e_deflate,
  
  /* PARSER FLAGS */
  http_pf_method = 1,
  http_pf_status_code = 1,
  http_pf_reason_phrase,
  http_pf_path = 2,
  http_pf_version = 4,
  http_pf_headers = 8,
  http_pf_body = 16,
  
  /* STATUS CODES */
  http_s_continue = 100,
  http_s_switching_protocols,
  http_s_processing,
  
  http_s_ok = 200,
  http_s_created,
  http_s_accepted,
  http_s_non_authoritative_information,
  http_s_no_content,
  http_s_reset_content,
  http_s_partial_content,
  http_s_multi_status,
  http_s_already_reported,
  http_s_im_used = 226,
  
  http_s_multiple_choices = 300,
  http_s_moved_permanently,
  http_s_found,
  http_s_see_other,
  http_s_not_modified,
  http_s_use_proxy,
  http_s_temporary_redirect = 307,
  http_s_permanent_redirect,
  
  http_s_bad_request = 400,
  http_s_unauthorized,
  http_s_payment_required,
  http_s_forbidden,
  http_s_not_found,
  http_s_method_not_allowed,
  http_s_not_acceptable,
  http_s_proxy_authentication_required,
  http_s_request_timeout,
  http_s_conflict,
  http_s_gone,
  http_s_length_required,
  http_s_precondition_failed,
  http_s_payload_too_large,
  http_s_request_uri_too_long,
  http_s_unsupported_media_type,
  http_s_requested_range_not_satisfiable,
  http_s_expectation_failed,
  http_s_i_am_a_teapot,
  http_s_misdirected_request = 421,
  http_s_unprocessable_entity,
  http_s_locked,
  http_s_failed_dependency,
  http_s_upgrade_required = 426,
  http_s_precondition_required = 428,
  http_s_too_many_requests,
  http_s_request_header_fields_too_large = 431,
  http_s_connection_closed_without_response = 444,
  http_s_unavailable_for_legal_reasons = 451,
  http_s_client_closed_request = 499,
  
  http_s_internal_server_error = 500,
  http_s_not_implemented,
  http_s_bad_gateway,
  http_s_service_unavailable,
  http_s_gateway_timeout,
  http_s_http_version_not_supported,
  http_s_variant_also_negotiates,
  http_s_insufficient_storage,
  http_s_loop_detected,
  http_s_not_extended = 510,
  http_s_network_authentication_required,
  http_s_network_connect_timeout_error = 599
};

#define IS_DIGIT(a) ((a) >= '0' && (a) <= '9')
#define IS_HEX(a) (IS_DIGIT(a) || ((a) >= 'A' && (a) <= 'F') || ((a) >= 'a' && (a) <= 'f'))

struct http_header {
  char* name;
  char* value;
  uint32_t name_len;
  uint32_t value_len;
};

enum http_parser_consts {
  /* LAST_AT */
  http_pla_method = 0,
  http_pla_res_version = 0,
  http_pla_status_code,
  http_pla_path = 1,
  http_pla_reason_phrase,
  http_pla_version = 2,
  http_pla_headers,
  http_pla_body,
  
  /* RETURN VALUES / ERRORS */
  http_valid = 0,
  http_incomplete,
  http_no_method,
  http_method_too_long,
  http_invalid_character,
  http_no_path,
  http_path_too_long,
  http_invalid_version,
  http_has_headers_but_max_is_0,
  http_too_many_headers,
  http_header_name_too_long,
  http_no_header_name,
  http_header_value_too_long,
  http_body_too_long,
  http_transfer_not_supported,
  http_encoding_not_supported,
  http_out_of_memory,
  http_corrupted_body_compression,
  http_no_chunk_size,
  http_invalid_status_code,
  http_reason_phrase_too_long
};

extern const char* http1_parser_strerror(const int);

struct http_message {
  union {
    char* method;
    char* reason_phrase;
  };
  char* path;
  struct http_header* headers;
  char* body;
  
  union {
    uint32_t method_len:8;
    uint32_t reason_phrase_len:8;
  };
  uint32_t headers_len:8;
  union {
    uint32_t path_len:16;
    uint32_t status_code:16;
  };
  uint32_t version:2;
  uint32_t transfer:1;
  uint32_t encoding:2;
  uint32_t allocated_body:1;
  uint32_t __unused:26;
  uint32_t body_len;
};

struct http_parser_session {
  union {
    uint32_t parsed_method:1;
    uint32_t parsed_status_code:1;
  };
  union {
    uint32_t parsed_path:1;
    uint32_t parsed_reason_phrase:1;
  };
  uint32_t parsed_version:1;
  uint32_t parsed_headers:1;
  uint32_t parsed_body:1;
  uint32_t last_at:3;
  uint32_t last_header_idx:8;
  uint32_t _unused:16;
  uint32_t last_idx;
};

struct http_parser_settings {
  union {
    uint32_t stop_at_method:1;
    uint32_t stop_at_status_code:1;
  };
  union {
    uint32_t stop_at_path:1;
    uint32_t stop_at_reason_phrase:1;
  };
  uint32_t stop_at_version:1;
  uint32_t stop_at_headers:1;
  uint32_t stop_at_every_header:1;
  
  uint32_t no_character_validation:1;
  uint32_t no_body_parsing:1;
  uint32_t _unused1:1;
  
  union {
    uint32_t max_method_len:8;
    uint32_t max_reason_phrase_len:8;
  };
  uint32_t max_headers:8;
  uint32_t max_path_len:16;
  
  uint32_t max_header_value_len:16;
  uint32_t max_header_name_len:8;
  uint32_t _unused2:8;
  
  uint32_t max_body_len;
};

extern uint32_t http1_message_length(const struct http_message* const);

extern void http1_create_message(char*, const struct http_message* const);

extern int http1_parse_request(char*, uint32_t, struct http_parser_session* const,
const struct http_parser_settings* const, struct http_message* const);

extern int http1_parse_response(char*, uint32_t, struct http_parser_session* const,
const struct http_parser_settings* const, struct http_message* const);

extern struct http_header* http1_seek_header(const struct http_message* const, const char* const, const uint32_t);

#endif // sKfWRTuG___9H8Dcb_vfV_0___tcQl_u