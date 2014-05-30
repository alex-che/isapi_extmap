isapi_extmap - an ISAPI Extension Mapping Filter for Windows CE HTTPD server.

isapi_extmap is created to add support for file extensions mapping to the 
standard Windows CE HTTPD server. 

During an HTTP request, HTTPD maps the request URL to some path on disk. 
Then it calls the isapi_extmap, which in turn can change file extension
in the path according to it's settings. So, the full path mapping goes
through the following path:
/some/url/file.ext -> \Disk\url\file.ext -> \Disk\url\file.new

Specifically, this adds support for extension-less URLs to HTTPD, since
isapi_extmap supports empty extensions in it's settings. In this case
the mapping goes this way:
/some/url/file -> \Disk\url\file -> \Disk\url\file.new

This is useful, if we have an ISAPI Extension, like asp.dll, which is 
mapped to some (say, .asp) file extension in HTTPD/ScriptMap registry 
settings (which do not support mapping files w/o extensions to any ISAPI 
extension). In this case, if we want our ASP URLs to be extension-less, 
then we can use the isapi_extmap component.


To add the isapi_extmap filter component to the HTTPD server, you need
to add the following registry entry (path may be different):

[HKEY_LOCAL_MACHINE\Comm\HTTPD]
"Filter DLLs"="\\Hard Disk\\isapi_extmap\\isapi_extmap.dll"

To set the isapi_extmap component extensions mapping, add the entries
to the HTTPD\ExtMap key, like the following:

[HKEY_LOCAL_MACHINE\Comm\HTTPD\ExtMap]
".empty"="."
".1"=".23"
"."=".asp"

This means the isapi_extmap will map file extensions the following way:
.empty         -> (no extension)
.1             -> .23
(no extension) -> .asp
