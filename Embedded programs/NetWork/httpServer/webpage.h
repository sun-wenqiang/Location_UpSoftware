#ifndef _WEB_PAGE_H
#define _WEB_PAGE_H

#define HtmlFile \
"<!DOCTYPE html>"\
"<html>"\
"<head>"\
"<meta charset=utf-8 />"\
"<title> Device Configure</title>"\
"<style>"\
"label{display:inline-block;width:4em;margin-right:1em;text-align:right;}"\
"input[type=submit]{width:20em;padding:9px;border-radius:3px;margin-left:1em;}"\
"input[type=text],select{width:15em;margin-bottom:9px;}"\
"div{background-color:#E0E0E0;border-radius:9px;top:15em;left:30em;position:absolute;padding:9px;}"\
"</style>"\
"</head>"\
"<body>"\
"<div>"\
"<form method=post >"\
"<label>IP:</label><input type=text name=xsip value=%d.%d.%d.%d /><br>"\
"<label>PORT:</label><input type=text name=xsport value=%d /><br>"\
"<label>GW:</label><input type=text name=xsgw value=%d.%d.%d.%d /><br>"\
"<label>MAC:</label><input type=text name=xsmac value=%02x-%02x-%02x-%02x-%02x-%02x /><br>"\
"<label>FS(khz):</label><select name=fs >"\
"<option value=0_selected >20</option><option value=1_selected >8</option>"\
"</select><br>"\
"<input type=submit value='Save and Restart' />"\
"</form>"\
"</div>"\
"</body>"\
"</html>"
#endif

#define ConfigTip \
"HTTP/1.1 400 OK\r\nContent-Type: text/html\r\nContent-Length: 50\r\n\r\n<HTML>\r\n<BODY>\r\%s\r\n</BODY>\r\n</HTML>\r\n\0"
