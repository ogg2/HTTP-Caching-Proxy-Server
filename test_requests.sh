#!/bin/bash

mkdir -p test-responses

echo "GET Request"
curl http://www.acme.com/ -x 0.0.0.0:12345 -o test-responses/get-response.txt

echo "Chunked GET Request"
curl http://www.httpwatch.com/httpgallery/chunked/chunkedimage.aspx -x 0.0.0.0:12345 -o test-responses/chunked-response.txt

echo "Max-Age GET Request"
curl http://www.artsci.utoronto.ca/futurestudents -x 0.0.0.0:12345 -o test-responses/max-age-response.txt
curl http://www.artsci.utoronto.ca/futurestudents -x 0.0.0.0:12345 -o test-responses/max-age-response.txt

echo "POST Request"
post_content="custname=a&custtel=123&custemail=a%40a.com&size=medium&topping=cheese&delivery=18%3A00&comments=none"
curl -d $(post_content) -X POST http://httpbin.org/forms/post -o test-responses/post-response.txt

echo "CONNECT Request"
curl https://www.google.com/ -x 0.0.0.0:12345 -o test-responses/connect-response.txt
