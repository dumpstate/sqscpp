#!/usr/bin/env python3


import http.client as http_client
import json
import logging
import requests

from argparse import ArgumentParser, Namespace
from typing import Any
from uuid import uuid4


def parse_args():
    parser = ArgumentParser(description="sqscpp test CLI")

    parser.add_argument(
        "command",
        help="Command to run",
        choices=[
            "smoke-test",
            "create-queue",
            "delete-queue",
            "get-queue-url",
            "list-queue-tags",
            "list-queues",
            "tag-queue",
            "untag-queue",
            "send-message",
            "purge-queue",
            "receive-message",
            "delete-message",
            "test-seed",
        ],
    )
    parser.add_argument(
        "--debug",
        help="Enable debug logging",
        action="store_true",
        default=False,
    )
    parser.add_argument(
        "--protocol",
        help="SQSCPP protocol",
        default="http",
        type=str,
    )
    parser.add_argument(
        "--host",
        help="SQSCPP host",
        default="localhost",
        type=str,
    )
    parser.add_argument(
        "--port",
        help="SQSCPP port",
        default=8080,
        type=int,
    )
    parser.add_argument(
        "--queue-name",
        help="Queue name",
        type=str,
    )
    parser.add_argument(
        "--queue-url",
        help="Queue URL",
        type=str,
    )
    parser.add_argument(
        "--tag",
        help="Tag",
        type=str,
    )
    parser.add_argument(
        "--receipt-handle",
        help="Receipt handle",
        type=str,
    )

    return parser.parse_args()


def base_url(args: Namespace):
    return f"{args.protocol}://{args.host}:{args.port}"


def call_sqs(url: str, action: str, body: Any):
    return requests.post(
        url,
        headers={
            "Content-Type": "application/x-amz-json-1.0",
            "X-Amz-Target": f"AmazonSQS.{action}",
            "X-Amzn-Trace-Id": str(uuid4()),
        },
        json=body,
    )


def create_queue(url: str, name: str):
    return call_sqs(url, "CreateQueue", {
        "QueueName": name,
        "Attributes": {
            "source": "cli.py",
        },
    })


def delete_queue(url: str, qurl: str):
    return call_sqs(url, "DeleteQueue", {"QueueUrl": qurl})


def get_queue_url(url: str, name: str):
    res = call_sqs(url, "GetQueueUrl", {"QueueName": name})
    res.raise_for_status()
    return res.json()["QueueUrl"]


def list_queue_tags(url: str, qurl: str):
    res = call_sqs(url, "ListQueueTags", {"QueueUrl": qurl})
    res.raise_for_status()
    return res.json()["Tags"]


def list_queues(url: str):
    res = call_sqs(url, "ListQueues", {})
    res.raise_for_status()
    return res.json()["QueueUrls"]


def tag_queue(url: str, qurl: str, tags: dict):
    return call_sqs(url, "TagQueue", {
        "QueueUrl": qurl,
        "Tags": tags,
    })


def untag_queue(url: str, qurl: str, tag_keys: list[str]):
    return call_sqs(url, "UntagQueue", {
        "QueueUrl": qurl,
        "TagKeys": tag_keys,
    })


def send_message(url: str, qurl: str, message: dict):
    res = call_sqs(url, "SendMessage", {
        "QueueUrl": qurl,
        "MessageBody": json.dumps(message),
    })
    print(res.json())
    return res


def purge_queue(url: str, qurl: str):
    return call_sqs(url, "PurgeQueue", {"QueueUrl": qurl})


def receive_message(url: str, qurl: str):
    res = call_sqs(url, "ReceiveMessage", {"QueueUrl": qurl})
    print(res.json())
    return res


def delete_message(url: str, qurl: str, receipt_handle: str):
    return call_sqs(url, "DeleteMessage", {
        "QueueUrl": qurl,
        "ReceiptHandle": receipt_handle,
    })


def test_seed(args: Namespace):
    url = base_url(args)
    qnames = [f"test-queue-{ix}" for ix in range(5)]

    for qname in qnames:
        create_queue(url, qname)
        tag_queue(url, get_queue_url(url, qname), {"key": "value"})
        send_message(url, get_queue_url(url, qname), {"message": "body"})


def smoke_test(args: Namespace):
    url = base_url(args)
    qnames = [f"test-{uuid4()}" for _ in range(4)]

    for qname in qnames:
        print(f"Creating queue {qname}")
        create_queue(url, qname)
    print(list_queues(url))

    qname = qnames[0]
    tag_queue(url, get_queue_url(url, qname), {"key": "value"})
    print(list_queue_tags(url, get_queue_url(url, qname)))
    untag_queue(url, get_queue_url(url, qname), ["key"])
    print(list_queue_tags(url, get_queue_url(url, qname)))

    for qname in qnames:
        print(f"Sending message to {qname}")
        send_message(url, get_queue_url(url, qname), {"key": "value"})

    for qname in qnames:
        print(f"Purging {qname}")
        purge_queue(url, get_queue_url(url, qname))

    for qname in qnames:
        print(f"Sending message to {qname}")
        send_message(url, get_queue_url(url, qname), {"key": "value"})
        print(f"Receiving message from {qname}")
        res = receive_message(url, get_queue_url(url, qname))
        receipt_handle = res.json()["Messages"][0]["ReceiptHandle"]
        print(f"Deleting message from {qname}")
        delete_message(url, get_queue_url(url, qname), receipt_handle)

    for qname in qnames:
        print(f"Deleting queue {qname}")
        qurl = get_queue_url(url, qname)
        delete_queue(url, qurl)
    print(list_queues(url))


def main():
    args = parse_args()
    print(args)

    if args.debug:
        http_client.HTTPConnection.debuglevel = 1
        logging.basicConfig()
        logging.getLogger().setLevel(logging.DEBUG)
        requests_log = logging.getLogger("requests.packages.urllib3")
        requests_log.setLevel(logging.DEBUG)
        requests_log.propagate = True

    if args.command == "smoke-test":
        smoke_test(args)
    elif args.command == "test-seed":
        test_seed(args)
    elif args.command == "create-queue":
        if not args.queue_name:
            raise ValueError("Queue name is required")
        create_queue(base_url(args), args.queue_name)
    elif args.command == "delete-queue":
        if not args.queue_url:
            raise ValueError("Queue URL is required")
        delete_queue(base_url(args), args.queue_url)
    elif args.command == "get-queue-url":
        if not args.queue_name:
            raise ValueError("Queue name is required")
        print(get_queue_url(base_url(args), args.queue_name))
    elif args.command == "list-queue-tags":
        if not args.queue_url:
            raise ValueError("Queue URL is required")
        print(list_queue_tags(base_url(args), args.queue_url))
    elif args.command == "list-queues":
        print(list_queues(base_url(args)))
    elif args.command == "tag-queue":
        if not args.queue_url:
            raise ValueError("Queue URL is required")
        if not args.tag:
            raise ValueError("Tag is required")
        tag_queue(base_url(args), args.queue_url, {args.tag: "value"})
    elif args.command == "untag-queue":
        if not args.queue_url:
            raise ValueError("Queue URL is required")
        if not args.tag:
            raise ValueError("Tag is required")
        untag_queue(base_url(args), args.queue_url, [args.tag])
    elif args.command == "send-message":
        if not args.queue_url:
            raise ValueError("Queue URL is required")
        send_message(base_url(args), args.queue_url, {"key": "value"})
    elif args.command == "purge-queue":
        if not args.queue_url:
            raise ValueError("Queue URL is required")
        purge_queue(base_url(args), args.queue_url)
    elif args.command == "receive-message":
        if not args.queue_url:
            raise ValueError("Queue URL is required")
        receive_message(base_url(args), args.queue_url)
    elif args.command == "delete-message":
        if not args.queue_url:
            raise ValueError("Queue URL is required")
        if not args.receipt_handle:
            raise ValueError("Receipt handle is required")
        delete_message(base_url(args), args.queue_url, args.receipt_handle)


if __name__ == "__main__":
    main()
