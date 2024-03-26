#!/usr/bin/env bash

set -euxo pipefail

function main {
    local target=$1
    local queue_name=$2
    curl -v \
        -H "Content-Type: application/x-amz-json-1.0" \
        -H "x-amz-target: AmazonSQS.GetQueueUrl" \
        "${target}" \
        -d "{\"QueueName\":\"${queue_name}\"}"
}

main "$@"
