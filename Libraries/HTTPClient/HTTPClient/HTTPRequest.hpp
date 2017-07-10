


enum class RequestStatus
{
	/** Has not been started via ProcessRequest() */
	NotStarted,
	/** Finished and was successful */
	Succeeded,
	/** Currently being ticked and processed */
	Processing,
	/** The request has been canceled*/
	Canceled,
	/** The request time-out*/
	TimeOut,
	/** Finished but failed */
	Failed,
	/** Failed because it was unable to connect (safe to retry) */
	Failed_ConnectionError

};


enum class RequestMethodType
{
	GET,
	POST,
	PUT,
	HEAD,
	DELETE,
	UNKNOWN,
};