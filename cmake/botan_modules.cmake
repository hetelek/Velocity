# Centralized list of Botan modules required by Velocity/XboxInternals.
# Trim to the minimum; update if code evolves.
set(VELOCITY_BOTAN_MODULES
    sha1
    hmac
    rc4
    rsa
    pubkey
    bigint
    auto_rng
    system_rng
    sha2_32
    asn1
    base64
    hex
    pem
    pk_pad
    emsa_pkcs1 # EMSA-PKCS1-v1_5 padding (aka EMSA3) for RSA signing (used in XContentHeader::ResignHeader)
)
