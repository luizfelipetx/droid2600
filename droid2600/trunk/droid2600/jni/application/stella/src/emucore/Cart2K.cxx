//============================================================================
//
//   SSSS    tt          lll  lll       
//  SS  SS   tt           ll   ll        
//  SS     tttttt  eeee   ll   ll   aaaa 
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2010 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: Cart2K.cxx,v 1.1.1.1 2010-10-26 03:44:58 cvs Exp $
//============================================================================

#include <cassert>
#include <cstring>

#include "System.hxx"
#include "Cart2K.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Cartridge2K::Cartridge2K(const uInt8* image, uInt32 size)
{
  // Size can be a maximum of 2K
  if(size > 2048) size = 2048;

  // Set image size to closest power-of-two for the given size
  mySize = 1;
  while(mySize < size)
    mySize <<= 1;

  // The smallest addressable area by Stella is 64 bytes
  // This should really be read from the System, but for now I'm going
  // to cheat a little and hard-code it to 64 (aka 2^6)
  if(mySize < 64)
    mySize = 64;

  // Initialize ROM with illegal 6502 opcode that causes a real 6502 to jam
  myImage = new uInt8[mySize];
  memset(myImage, 0x02, mySize);

  // Copy the ROM image into my buffer
  memcpy(myImage, image, size);

  // Set mask for accessing the image buffer
  // This is guaranteed to work, as mySize is a power of two
  myMask = mySize - 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Cartridge2K::~Cartridge2K()
{
  delete[] myImage;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Cartridge2K::reset()
{
  myBankChanged = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Cartridge2K::install(System& system)
{
  mySystem = &system;
  uInt16 shift = mySystem->pageShift();
  uInt16 mask = mySystem->pageMask();

  // Make sure the system we're being installed in has a page size that'll work
  assert((0x1000 & mask) == 0);

  // Map ROM image into the system
  System::PageAccess access;
  access.directPokeBase = 0;
  access.device = this;
  access.type = System::PA_READ;

  for(uInt32 address = 0x1000; address < 0x2000; address += (1 << shift))
  {
    access.directPeekBase = &myImage[address & myMask];
    mySystem->setPageAccess(address >> shift, access);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt8 Cartridge2K::peek(uInt16 address)
{
  return myImage[address & myMask];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Cartridge2K::poke(uInt16, uInt8)
{
  // This is ROM so poking has no effect :-)
  return false;
} 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Cartridge2K::bank(uInt16 bank)
{
  // Doesn't support bankswitching
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt16 Cartridge2K::bank() const
{
  // Doesn't support bankswitching
  return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt16 Cartridge2K::bankCount() const
{
  return 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Cartridge2K::patch(uInt16 address, uInt8 value)
{
  myImage[address & myMask] = value;
  return myBankChanged = true;
} 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const uInt8* Cartridge2K::getImage(int& size) const
{
  size = mySize;
  return myImage;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Cartridge2K::save(Serializer& out) const
{

    out.putString(name());



  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Cartridge2K::load(Serializer& in)
{

    if(in.getString() != name())
      return false;


  return true;
}
