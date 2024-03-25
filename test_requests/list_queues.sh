#!/usr/bin/env bash

set -euxo pipefail

function main {
    local target=$1
    curl -v \
        -H "Content-Type: application/x-amz-json-1.0" \
        -H "x-amz-target: AmazonSQS.ListQueues" \
        "${target}" \
        -d '{}'
}

main "$@"
