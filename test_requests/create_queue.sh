#!/usr/bin/env bash

set -euxo pipefail

function main {
    local target=$1
    local queue_name=$2
    curl -v \
        -H "content-type: application/x-amz-json-1.0" \
        -H "x-amz-target: AmazonSQS.CreateQueue" \
        "${target}" \
        -d "{\"QueueName\": \"${queue_name}\"}"
}

main "$@"
