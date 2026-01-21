FROM alpine:latest

RUN apk add --no-cache ca-certificates

WORKDIR /app

COPY ./server/main /app/

RUN chmod +x /app/main

EXPOSE 8080

# 執行 server
CMD ["./main"]
