#!/usr/bin/env python3

import argparse
import base64
import block_pb2
import os
import sys

from OpenSSL import crypto
from google.protobuf import json_format


def load_genesis_block(genesis_block_path):
  with open(genesis_block_path) as inp:
    return json_format.Parse(inp.read(), block_pb2.Block())


def iterate_all_commands(block):
  for tx in block.block_v1.payload.transactions:
    for cmd in tx.payload.reduced_payload.commands:
      yield cmd


def get_added_peers(block):
  return map(lambda cmd: cmd.add_peer.peer,
            [cmd for cmd in iterate_all_commands(block) if cmd.WhichOneof('command') == 'add_peer'])


def format_dir(peer):
  return '{}-{}'.format(peer.address, peer.peer_key[:5])


def make_key():
  key = crypto.PKey()
  key.generate_key(type=crypto.TYPE_RSA, bits=1024)
  return key


def make_cert(tls_key, peer_key):
  split_key = list()
  tail = peer_key
  while tail:
    split_key.append(tail[:63])
    tail = tail[63:]
  dns_key = 'iroha-node-public-key.' + '.'.join(split_key)
  cert = crypto.X509()
  cert.get_subject().C = 'JP'
  cert.get_subject().ST = 'ONE WORLD'
  cert.get_subject().L = 'ONE ECONOMY'
  cert.get_subject().O = 'Soramitsu Ltd'
  cert.get_subject().OU = 'test'
  cert.get_subject().CN = 'iroha peer {}'.format(peer_key[:5])
  cert.add_extensions([crypto.X509Extension(b'subjectAltName', critical=True,
                                            value=b'DNS:iroha, DNS:' + dns_key.encode())])
  cert.set_serial_number(1)
  cert.gmtime_adj_notBefore(0)
  cert.gmtime_adj_notAfter(365*24*60*60)
  cert.set_issuer(cert.get_subject())
  cert.set_pubkey(tls_key)
  cert.sign(tls_key, 'sha1')
  return cert


if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('-g', '--genesis-block',
                      help='genesis block path', required=True)
  parser.add_argument('-o', '--output-path',
                      help='path to write credentials to', default='.')
  args = parser.parse_args()

  assert(os.path.isfile(args.genesis_block))
  assert(os.path.isdir(args.output_path))

  genesis_block = load_genesis_block(args.genesis_block)
  for peer in get_added_peers(genesis_block):
    creds_dir = os.path.join(args.output_path, format_dir(peer))
    if not os.path.isdir(creds_dir):
      os.mkdir(creds_dir)
    key = make_key()
    cert = make_cert(key, peer.peer_key)
    peer.tls_certificate = crypto.dump_certificate(crypto.FILETYPE_PEM, cert)
    #peer.tls_certificate = base64.encodebytes(
    #    crypto.dump_certificate(crypto.FILETYPE_ASN1, cert)).strip()
    with open(os.path.join(creds_dir, 'tls.key'), 'wb') as out:
      out.write(crypto.dump_privatekey(crypto.FILETYPE_PEM, key))
    with open(os.path.join(creds_dir, 'tls.crt'), 'wb') as out:
      out.write(crypto.dump_certificate(crypto.FILETYPE_PEM, cert))

  with open(os.path.join(args.output_path, 'genesis_with_certs'), 'wt') as out:
    out.write(json_format.MessageToJson(genesis_block))
