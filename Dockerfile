    ARG VARIANT=release
ARG TARGETARCH=amd64

FROM ghcr.io/statecl/framework:latest-builder-${TARGETARCH}-${VARIANT} AS builder

WORKDIR /src
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j$(nproc)

FROM alpine:3.18

RUN apk add --no-cache libstdc++ libgcc
COPY --from=builder /src/build/app /app/app

EXPOSE 8080
ENTRYPOINT ["/app/app"]
