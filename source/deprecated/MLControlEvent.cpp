//
//  MLControlEvent.cpp
//  madronalib
//
//  Created by Randy Jones on 7/9/14.
//:
//

#include "MLControlEvent.h"

MLControlEvent::MLControlEvent() :
mTime(0),
mType(kNull),
mChannel(0),
mID(0),
mValue1(0.),
mValue2(0.),
mValue3(0.),
mValue4(0.)
{
}

MLControlEvent::MLControlEvent(MLControlEvent::EventType type, int channel, int id, uint64_t time, float value, float value2, float value3, float value4) :
mTime(time),
mType(type),
mChannel(channel),
mID(id),
mValue1(value),
mValue2(value2),
mValue3(value3),
mValue4(value4)
{
}

void MLControlEvent::clear()
{
  *this = kMLNullControlEvent;
}

int MLControlEventVector::findFreeEvent() const
{
  int r = -1;
  for(int i=0; i<size(); ++i)
  {
    if (at(i).isFree())
    {
      r = i;
      break;
    }
  }
  return r;
}

void MLControlEventVector::clearEventsMatchingID(int id)
{
  for(int i=0; i<size(); ++i)
  {
    MLControlEvent& event = (*this)[i];
    if (event.mID == id)
    {
      event.clear();
    }
  }
}

void MLControlEventStack::push(const MLControlEvent& e)
{
  if(mSize < size())
  {
    (*this)[mSize++] = e;
  }
}

MLControlEvent MLControlEventStack::pop()
{
  if(mSize > 0)
  {
    mSize--;
    return (*this)[mSize];
  }
  return kMLNullControlEvent;
}

bool MLControlEventStack::isEmpty()
{
  return (mSize == 0);
}

int MLControlEventStack::getSize()
{
  return mSize;
}

void MLControlEventStack::clearEventsMatchingID(int id)
{
  int currentSize = mSize;
  for(int i=0; i<currentSize; ++i)
  {
    MLControlEvent& event = (*this)[i];
    if (event.mID == id)
    {
      event.clear();
      // compact
      for(int j=i; j<currentSize - 1; ++j)
      {
        (*this)[j] = (*this)[j + 1];
      }
      (*this)[currentSize - 1] = kMLNullControlEvent;
      
      mSize--;
      if(mSize <= 0) return;
    }
  }
}


