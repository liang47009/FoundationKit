
#include <vector>
#include <unordered_map>
#include <memory>
#include "FoundationKit/FoundationMacros.h"
#include "FoundationKit/Base/Types.h"

NS_FK_BEGIN
class HTTPRequest;
class HTTPResponse
{
    friend class HTTPRequest;
    HTTPResponse() = delete;
public:
    typedef std::unordered_map<std::string, std::string> KeyValueMap;
    typedef std::shared_ptr<HTTPResponse>                Pointer;
    typedef std::shared_ptr<HTTPRequest>                 RequestPtr;
    typedef std::weak_ptr<HTTPRequest>                   RequestWeakPtr;

public:
    HTTPResponse(RequestPtr pRequest);
    ~HTTPResponse();
    std::string              getURL();
    std::string              getURLParameter(const std::string& parameterName);
    std::string              getHeader(const std::string& headerName);
    std::vector<std::string> getAllHeaders();
    std::string              getContentType();
    size_t                   getContentSize();
    std::vector<uint8>&      getResponseData();
    int32                    getResponseCode();
    std::string              getCookies();
    std::string              getPerformMsg();
    std::string              getErrorMsg();
    bool                     isReady();
    bool                     isSucceeded();

protected:
    HTTPResponse&            setHeader(const std::string& headerName, const std::string& headerValue);
    HTTPResponse&            setContentSize(int contentSize);
    HTTPResponse&            setCookies(const std::string& cookies);
    HTTPResponse&            setReady(bool bReady);
    HTTPResponse&            setSucceeded(bool bSucceeded);
    HTTPResponse&            setPerformMsg(const std::string& performMsg);
    HTTPResponse&            setErrorMsg(const std::string& errorMsg);

private:
    RequestWeakPtr     _request;
    /** BYTE array to fill in as the response is read via didReceiveData */
    std::vector<uint8> _responseData;
    /** Cached code from completed response */
    int32              _responseCode;
    /** Cached key/value header pairs. Parsed once request completes */
    KeyValueMap        _headers;
    /** message for http code */
    std::string        _responseMessage;
    /** Cached content length from completed response */
    int32              _contentSize;
    /** True when the response has finished async processing */
    int32 volatile     _bIsReady;
    /** True if the response was successfully received/processed */
    int32 volatile     _bSucceeded;
    std::string        _cookies;
    std::string        _performMsg;
    std::string        _errorMsg;
};

NS_FK_END




