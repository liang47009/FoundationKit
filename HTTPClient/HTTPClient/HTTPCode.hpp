/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_HTTPCODE_HPP
#define FOUNDATIONKIT_HTTPCODE_HPP

namespace HTTPCode
{

struct HTTPCodePair
{
    unsigned code;
    const char *const description;
};

/*/ 0XX 'route66'
/*/
extern const HTTPCodePair SHUTDOWN;
/*/ 1XX 'informational'
/*/
extern const HTTPCodePair CONTINUE;
extern const HTTPCodePair SWITCH_PROTOCOLS;
/*/ 2XX 'success'
/*/
extern const HTTPCodePair OK;
extern const HTTPCodePair CREATED;
extern const HTTPCodePair ACCEPTED;
extern const HTTPCodePair PARTIAL;
extern const HTTPCodePair NO_CONTENT;
extern const HTTPCodePair RESET_CONTENT;
extern const HTTPCodePair PARTIAL_CONTENT;
extern const HTTPCodePair WEBDAV_MULTI_STATUS;
/*/ 3XX 'redirection'
/*/
extern const HTTPCodePair AMBIGUOUS;
extern const HTTPCodePair MOVED;
extern const HTTPCodePair REDIRECT;
extern const HTTPCodePair REDIRECT_METHOD;
extern const HTTPCodePair NOT_MODIFIED;
extern const HTTPCodePair USE_PROXY;
extern const HTTPCodePair REDIRECT_KEEP_VERB;
/*/ 4XX 'client error'
/*/
extern const HTTPCodePair BAD_REQUEST;
extern const HTTPCodePair DENIED;
extern const HTTPCodePair PAYMENT_REQ;
extern const HTTPCodePair FORBIDDEN;
extern const HTTPCodePair NOT_FOUND;
extern const HTTPCodePair BAD_METHOD;
extern const HTTPCodePair NONE_ACCEPTABLE;
extern const HTTPCodePair PROXY_AUTH_REQ;
extern const HTTPCodePair REQUEST_TIMEOUT;
extern const HTTPCodePair CONFLICT;
extern const HTTPCodePair GONE;
extern const HTTPCodePair LENGTH_REQUIRED;
extern const HTTPCodePair PRECOND_FAILED;
extern const HTTPCodePair REQUEST_TOO_LARGE;
extern const HTTPCodePair URI_TOO_LONG;
extern const HTTPCodePair UNSUPPORTED_MEDIA;
extern const HTTPCodePair RETRY_WITH;
/*/ 5XX 'server error'
/*/
extern const HTTPCodePair SERVER_ERROR;
extern const HTTPCodePair NOT_SUPPORTED;
extern const HTTPCodePair BAD_GATEWAY;
extern const HTTPCodePair SERVICE_UNAVAIL;
extern const HTTPCodePair GATEWAY_TIMEOUT;
extern const HTTPCodePair VERSION_NOT_SUP;



/*/ 1XX informational
/*/ const HTTPCodePair SHUTDOWN = { 0, "Shutdown requested" };
/*/ 1XX informational
/*/ const HTTPCodePair CONTINUE = { 100, "The request can be continued." }; /*/
/*/ const HTTPCodePair SWITCH_PROTOCOLS = { 101, "The server has switched protocols in an upgrade header." };
/*/ 2XX success
/*/ const HTTPCodePair OK = { 200, "The request completed successfully." }; /*/
/*/ const HTTPCodePair CREATED = { 201, "The request has been fulfilled and resulted in the creation of a new resource." }; /*/
/*/ const HTTPCodePair ACCEPTED = { 202, "The request has been accepted for processing, but the processing has not been completed." }; /*/
/*/ const HTTPCodePair PARTIAL = { 203, "The returned meta information in the entity-header is not the definitive set available from the originating server." }; /*/
/*/ const HTTPCodePair NO_CONTENT = { 204, "The server has fulfilled the request, but there is no new information to send back." }; /*/
/*/ const HTTPCodePair RESET_CONTENT = { 205, "The request has been completed, and the client program should reset the document view that caused the request to be sent to allow the user to easily initiate another input action." }; /*/
/*/ const HTTPCodePair PARTIAL_CONTENT = { 206, "The server has fulfilled the partial GET request for the resource." }; /*/
/*/ const HTTPCodePair WEBDAV_MULTI_STATUS = { 207, "During a World Wide Web Distributed Authoring and Versioning (WebDAV) operation, this indicates multiple status const HTTPCodePairs for a single response. The response body contains Extensible Markup Language (XML) that describes the status const HTTPCodePairs. For more information, see HTTP Extensions for Distributed Authoring." };
/*/ 3XX redirection
/*/ const HTTPCodePair AMBIGUOUS = { 300, "The requested resource is available at one or more locations." }; /*/
/*/ const HTTPCodePair MOVED = { 301, "The requested resource has been assigned to a new permanent Uniform Resource Identifier (URI), and any future references to this resource should be done using one of the returned URIs." }; /*/
/*/ const HTTPCodePair REDIRECT = { 302, "The requested resource resides temporarily under a different URI." }; /*/
/*/ const HTTPCodePair REDIRECT_METHOD = { 303, "The response to the request can be found under a different URI and should be retrieved using a GET HTTP verb on that resource." }; /*/
/*/ const HTTPCodePair NOT_MODIFIED = { 304, "The requested resource has not been modified." }; /*/
/*/ const HTTPCodePair USE_PROXY = { 305, "The requested resource must be accessed through the proxy given by the location field." }; /*/
/*/ const HTTPCodePair REDIRECT_KEEP_VERB = { 307, "The redirected request keeps the same HTTP verb. HTTP/1.1 behavior." };
/*/ 4XX client's fault
/*/ const HTTPCodePair BAD_REQUEST = { 400, "The request could not be processed by the server due to invalid syntax." }; /*/
/*/ const HTTPCodePair DENIED = { 401, "The requested resource requires user authentication." }; /*/
/*/ const HTTPCodePair PAYMENT_REQ = { 402, "Not implemented in the HTTP protocol." }; /*/
/*/ const HTTPCodePair FORBIDDEN = { 403, "The server understood the request, but cannot fulfill it." }; /*/
/*/ const HTTPCodePair NOT_FOUND = { 404, "The server has not found anything that matches the requested URI." }; /*/
/*/ const HTTPCodePair BAD_METHOD = { 405, "The HTTP verb used is not allowed." }; /*/
/*/ const HTTPCodePair NONE_ACCEPTABLE = { 406, "No responses acceptable to the client were found." }; /*/
/*/ const HTTPCodePair PROXY_AUTH_REQ = { 407, "Proxy authentication required." }; /*/
/*/ const HTTPCodePair REQUEST_TIMEOUT = { 408, "The server timed out waiting for the request." }; /*/
/*/ const HTTPCodePair CONFLICT = { 409, "The request could not be completed due to a conflict with the current state of the resource. The user should resubmit with more information." }; /*/
/*/ const HTTPCodePair GONE = { 410, "The requested resource is no longer available at the server, and no forwarding address is known." }; /*/
/*/ const HTTPCodePair LENGTH_REQUIRED = { 411, "The server cannot accept the request without a defined content length." }; /*/
/*/ const HTTPCodePair PRECOND_FAILED = { 412, "The precondition given in one or more of the request header fields evaluated to false when it was tested on the server." }; /*/
/*/ const HTTPCodePair REQUEST_TOO_LARGE = { 413, "The server cannot process the request because the request entity is larger than the server is able to process." }; /*/
/*/ const HTTPCodePair URI_TOO_LONG = { 414, "The server cannot service the request because the request URI is longer than the server can interpret." }; /*/
/*/ const HTTPCodePair UNSUPPORTED_MEDIA = { 415, "The server cannot service the request because the entity of the request is in a format not supported by the requested resource for the requested method." }; /*/
/*/ const HTTPCodePair RETRY_WITH = { 449, "The request should be retried after doing the appropriate action." };
/*/ 5XX server's fault
/*/ const HTTPCodePair SERVER_ERROR = { 500, "The server encountered an unexpected condition that prevented it from fulfilling the request." }; /*/
/*/ const HTTPCodePair NOT_SUPPORTED = { 501, "The server does not support the functionality required to fulfill the request." }; /*/
/*/ const HTTPCodePair BAD_GATEWAY = { 502, "The server, while acting as a gateway or proxy, received an invalid response from the upstream server it accessed in attempting to fulfill the request." }; /*/
/*/ const HTTPCodePair SERVICE_UNAVAIL = { 503, "The service is temporarily overloaded." }; /*/
/*/ const HTTPCodePair GATEWAY_TIMEOUT = { 504, "The request was timed out waiting for a gateway." }; /*/
/*/ const HTTPCodePair VERSION_NOT_SUP = { 505, "The server does not support the HTTP protocol version that was used in the request message." };

 } // end namespace HTTPCode
#endif // FOUNDATIONKIT_HTTPCODE_HPP
