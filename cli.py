#!/usr/bin/env python3


import http.client as http_client
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
    return call_sqs(url, "CreateQueue", {"QueueName": name})


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


if __name__ == "__main__":
    main()
