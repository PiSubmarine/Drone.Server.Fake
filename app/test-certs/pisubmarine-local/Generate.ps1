$ErrorActionPreference = "Stop"

$certDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Push-Location $certDir

try
{
    & openssl req -x509 -newkey rsa:2048 -sha256 -days 825 -nodes `
        -keyout ca-key.pem `
        -out ca-cert.pem `
        -subj "/CN=PiSubmarine"

    & openssl req -new -newkey rsa:2048 -nodes `
        -keyout server-key.pem `
        -out server.csr `
        -config server-openssl.cnf

    & openssl x509 -req -in server.csr `
        -CA ca-cert.pem `
        -CAkey ca-key.pem `
        -CAcreateserial `
        -out server-cert.pem `
        -days 825 `
        -sha256 `
        -copy_extensions copy

    & openssl req -new -newkey rsa:2048 -nodes `
        -keyout client-key.pem `
        -out client.csr `
        -config client-openssl.cnf

    & openssl x509 -req -in client.csr `
        -CA ca-cert.pem `
        -CAkey ca-key.pem `
        -CAcreateserial `
        -out client-cert.pem `
        -days 825 `
        -sha256 `
        -copy_extensions copy

    Remove-Item -LiteralPath server.csr, client.csr, ca-cert.srl -ErrorAction SilentlyContinue
}
finally
{
    Pop-Location
}
