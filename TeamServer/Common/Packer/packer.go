package Packer

import (
	"AllSecure/TeamServer/Common"
	"AllSecure/TeamServer/Crypt"
	"bytes"
	"encoding/binary"
)

// very similar concept to the package code in the agent implant.
// where we create an array of bytes that is actually data to be sent from the agent to the server.
// or in this case, the configuration information given by the server to the agent.
type Packer struct {
	data []byte
	size int

	AesKey []byte
	AesIv  []byte
}

// constructor for the Packer struct. Takes in an AES key and IV and returns a pointer to a new Packer struct
// used when compiling the agent implant & is how we input our configuration for the agent.
func (p *Packer) NewPacker(AesKey, AesIV []byte) *Packer {
	var packer = new(Packer)
	packer.AesKey = AesKey
	packer.AesIv = AesIV
	return packer
}

// adds 8 bytes of data to the Packer struct
func (p *Packer) AddInt64(data int64) {
	var buffer = make([]byte, 0)
	binary.LittleEndian.PutUint64(buffer, uint64(data))
	p.data = append(p.data, buffer...)
	p.size += 8
}

// adds 8 bytes of data to the Packer struct
func (p *Packer) AddInt32(data int32) {
	var buffer = make([]byte, 0)
	binary.LittleEndian.PutUint32(buffer, uint32(data))
	p.data = append(p.data, buffer...)
	p.size += 4
}

func (p *Packer) AddInt(data int) {
	var buffer = make([]byte, 0)
	binary.LittleEndian.PutUint32(buffer, uint32(data))
	p.data = append(p.data, buffer...)
	p.size += 4
}

// adds 2 bytes of data to the packaer struct
func (p *Packer) AddInt16(data int16) {
	var buffer = make([]byte, 0)
	binary.LittleEndian.PutUint16(buffer, uint16(data))
	p.data = append(p.data, buffer...)
	p.size += 2
}

// adds 1 byte of data to the Packer struct
func (p *Packer) AddInt8(data int8) {
	p.data = append(p.data, byte(data))
	p.size++
}

// add 4 bytes of unsigned data to the Packer
func (p *Packer) AddUint32(data uint32) {
	var buffer = make([]byte, 0)
	binary.LittleEndian.PutUint32(buffer, data)
	p.data = append(p.data, buffer...)
	p.size += 4
}

func (p *Packer) AddString(data string) {
	p.AddBytes(Common.EncodeUTF8(data))
}

func (p *Packer) AddWString(data string) {
	p.AddBytes(Common.EncodeUTF16(data))
}

func (p *Packer) AddBytes(data []byte) {
	var buffer = make([]byte, 4)
	binary.LittleEndian.PutUint32(buffer, uint32(len(data))) // add size of data as prefix to buffer size.
	p.data = append(p.data, buffer...)
	p.data = append(p.data, data...)
	p.size += 4
	p.size += len(data)
}

func (p *Packer) Build() []byte {
	var temp = make([]byte, 32)

	if bytes.Compare(p.AesKey, temp) == 0 {
		return p.data
	}

	if (p.AesKey != nil) || (p.AesIv != nil) {
		p.data = Crypt.AES256CTR(p.data, p.AesKey, p.AesIv)
	}

	return p.data
}

func (p *Packer) GetBuffer() []byte {
	return p.data
}

func (p *Packer) GetSize() int {
	return p.size
}
