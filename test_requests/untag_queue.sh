#!/usr/bin/env bash

set -euxo pipefail

function main {
    local target=$1
    local queue_url=$2
    local key=$3
    curl -v \
        -H "Content-Type: application/x-amz-json-1.0" \
        -H "x-amz-target: AmazonSQS.UntagQueue" \
        "${target}" \
        -d '{
            "QueueUrl": "'${queue_url}'",
            "TagKeys": [
                "'${key}'"
            ]
        }'
}

main "$@"
