
#include <vector>
#include <unordered_map>
#include <memory>
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/Base/types.hpp"

NS_FK_BEGIN
namespace network{
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
    ~HTTPResponse();
    static Pointer           create(RequestPtr pRequest);
    std::string              getURL();
    std::string              getURLParameter(const std::string& parameterName);
    std::string              getHeader(const std::string& headerName);
    std::vector<std::string> getAllHeaders();
    std::string              getContentType();
    size_t                   getContentSize();
    std::vector<uint8>&      getContentData();
    long                     getResponseCode();
    std::string              getCookies();
    std::string              getResponseMsg();
    std::string              getErrorMsg();
    bool                     isReady();
    bool                     isSucceeded();
    void                     dumpInfo();
protected:
    HTTPResponse(RequestPtr pRequest);
    HTTPResponse&            setHeader(const std::string& headerName, const std::string& headerValue);
    HTTPResponse&            setContentSize(int contentSize);
    HTTPResponse&            setCookies(const std::string& cookies);
    HTTPResponse&            setReady(bool bReady);
    HTTPResponse&            setSucceeded(bool bSucceeded);
    HTTPResponse&            setResponseCode(long responseCode);
    HTTPResponse&            setResponseMsg(const std::string& responseMsg);
    HTTPResponse&            setErrorMsg(const std::string& errorMsg);

private:
    RequestWeakPtr     _request;
    /** BYTE array to fill in as the response is read via didReceiveData */
    std::vector<uint8> _contentData;
    /** Cached content length from completed response */
    int32              _contentSize;
    /** Cached code from completed response */
    long               _responseCode;
    /** Cached key/value header pairs. Parsed once request completes */
    KeyValueMap        _headers;
    /** message for http code */
    std::string        _responseMessage;
    /** True when the response has finished async processing */
    bool volatile      _bIsReady;
    /** True if the response was successfully received/processed */
    bool volatile      _bSucceeded;
    std::string        _cookies;
    std::string        _errorMsg;
};
} // namespace network
NS_FK_END




