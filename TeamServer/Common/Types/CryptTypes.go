package Types

import (
	"encoding/asn1"
	"math/big"
)

/*
ECCPublicKey ::= SEQUENCE {
	flags 		BIT STRING(0),
	keySzie 	INTEGER,
	pubKey.X 	INTEGER,
	pubKey.y 	INTEGER,
}
ECCPrivateKey ::= SEQUENCE {
	flags 		BIT STRING(1),
	keySzie 	INTEGER,
	pubKey.X 	INTEGER,
	pubKey.y 	INTEGER,
	secret.k	INTEGER,
}
*/

//TODO: look at these resources anytime you're stuck on asn1 encoding. Huge life saver.
//https://letsencrypt.org/docs/a-warm-welcome-to-asn1-and-der/#encoding-instructions
//https://lapo.it/asn1js/#MEuBAgAAggEggyAKkJeJdQWXdBnWpm90XY_S06EsDpnWqFhdL_Xv9wbwF4QgCCh8TNHukrPBuAz8OCFGzryXyibulK-pDZheHyjYNyE

type ECCPublicKey struct {
	Flags   asn1.BitString
	KeySize int
	PubKeyX *big.Int
	PubKeyY *big.Int
}

type ECCPrivateKey struct {
	Flags   asn1.BitString
	KeySize int
	PubkeyX *big.Int
	PubKeyY *big.Int
	SecretK *big.Int
}
