/*
* Copyright (c) 2008, AMT – The Association For Manufacturing Technology (“AMT”)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the AMT nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* DISCLAIMER OF WARRANTY. ALL MTCONNECT MATERIALS AND SPECIFICATIONS PROVIDED
* BY AMT, MTCONNECT OR ANY PARTICIPANT TO YOU OR ANY PARTY ARE PROVIDED "AS IS"
* AND WITHOUT ANY WARRANTY OF ANY KIND. AMT, MTCONNECT, AND EACH OF THEIR
* RESPECTIVE MEMBERS, OFFICERS, DIRECTORS, AFFILIATES, SPONSORS, AND AGENTS
* (COLLECTIVELY, THE "AMT PARTIES") AND PARTICIPANTS MAKE NO REPRESENTATION OR
* WARRANTY OF ANY KIND WHATSOEVER RELATING TO THESE MATERIALS, INCLUDING, WITHOUT
* LIMITATION, ANY EXPRESS OR IMPLIED WARRANTY OF NONINFRINGEMENT,
* MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. 

* LIMITATION OF LIABILITY. IN NO EVENT SHALL AMT, MTCONNECT, ANY OTHER AMT
* PARTY, OR ANY PARTICIPANT BE LIABLE FOR THE COST OF PROCURING SUBSTITUTE GOODS
* OR SERVICES, LOST PROFITS, LOSS OF USE, LOSS OF DATA OR ANY INCIDENTAL,
* CONSEQUENTIAL, INDIRECT, SPECIAL OR PUNITIVE DAMAGES OR OTHER DIRECT DAMAGES,
* WHETHER UNDER CONTRACT, TORT, WARRANTY OR OTHERWISE, ARISING IN ANY WAY OUT OF
* THIS AGREEMENT, USE OR INABILITY TO USE MTCONNECT MATERIALS, WHETHER OR NOT
* SUCH PARTY HAD ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES.
*/

#ifndef COMPONENT_EVENT_HPP
#define COMPONENT_EVENT_HPP

#include <string>
#include <cmath>

#include "component.hpp"
#include "globals.hpp"

class DataItem;
class ComponentEvent;

class ComponentEventPtr {
public:
  // Constructors
  ComponentEventPtr() { mEvent = NULL; }
  ComponentEventPtr(ComponentEventPtr &aPtr, bool aTakeRef = false) {
    mEvent = NULL;
    setObject(aPtr.getObject(), aTakeRef);
  }
  ComponentEventPtr(ComponentEvent &aEvent, bool aTakeRef = false) {
    mEvent = NULL;
    setObject(&aEvent, aTakeRef);
  }
  ComponentEventPtr(ComponentEvent *aEvent, bool aTakeRef = false) {
    mEvent = NULL;
    setObject(aEvent, aTakeRef);
  }
  
  
  // Destructor
  ~ComponentEventPtr();
  
  // Getters
  ComponentEvent *getObject() const { return mEvent; }
  ComponentEvent *operator->(void) const { return mEvent; }
  operator ComponentEvent*(void ) const { return mEvent; }
  
  // Setters
  ComponentEvent *setObject(ComponentEvent *aEvent, bool aTakeRef = false);
  ComponentEvent *operator=(ComponentEvent *aEvent) { return setObject(aEvent); }  
  ComponentEvent *operator=(ComponentEventPtr &aPtr) { return setObject(aPtr.getObject()); }
  
protected:
  ComponentEvent *mEvent;
};


/* Component Event */
class ComponentEvent
{
public:
  enum ELevel {
    NORMAL,
    WARNING,
    FAULT
  };
    
  static const unsigned int NumLevels = 3;
  static const std::string SLevels[];
  
public:
  /* Initialize with the data item reference, sequence number, time and value */
  ComponentEvent(
    DataItem& dataItem,
    unsigned int sequence,
    const std::string& time,
    const std::string& value
  );
  
  /* Copy constructor */
  ComponentEvent(ComponentEvent& ce);
  
  /* Extract the component event data into a map */
  std::map<std::string, std::string> *getAttributes();
  
  /* Get the data item associated with this event */
  DataItem * getDataItem() const { return mDataItem; }
  
  /* Get the value */
  const std::string &getValue() const { return mValue; }
  ELevel getLevel();
  const std::string &getLevelString() { return SLevels[getLevel()]; }
  
  unsigned int getSequence() const { return mSequence; }
  
  /* Reference count management */
  void referTo();
  void unrefer();
  
  unsigned int refCount() { return mRefCount; }
  
  ComponentEvent *getFirst();
  ComponentEvent *getPrev() { return mPrev; }
  void getList(std::list<ComponentEventPtr> &aList);
  void appendTo(ComponentEvent *aEvent) { mPrev = aEvent; }

protected:
  /* Virtual destructor */
  virtual ~ComponentEvent();
  
protected:
  /* Reference count */
  unsigned int mRefCount;
  
  /* Holds the data item from the device */
  DataItem * mDataItem;
  
  /* Sequence number of the event */
  unsigned int mSequence;
  
  /* Timestamp of the event's occurence */
  std::string mTime;
  
  /* Hold the alarm data: CODE|NATIVECODE|SEVERITY|STATE */
  /* or the Conditon data: SEVERITY|NATIVE_CODE|[SUB_TYPE] */
  std::string mAlarmData;
  ELevel mLevel;
  
  /* The value of the event, either as a float or a string */
  std::string mValue;
  bool mIsFloat;
  
  /* The attributes, created on demand */
  bool mHasAttributes;
  std::map<std::string, std::string> mAttributes;
  
  // For back linking of condition
  ComponentEventPtr mPrev;

protected:
  /* Convert the value to the agent unit standards */
  void convertValue(const std::string& value);
};

inline ComponentEventPtr::~ComponentEventPtr()
{
  if (mEvent)
    mEvent->unrefer();
}

inline ComponentEvent *ComponentEventPtr::setObject(ComponentEvent *aEvent, bool aTakeRef) {
  if (mEvent != NULL)
    mEvent->unrefer();
  mEvent = aEvent;
  if (aEvent != NULL && !aTakeRef)
    mEvent->referTo();
  
  return aEvent;
}

inline ComponentEvent::ELevel ComponentEvent::getLevel()
{
  if (!mHasAttributes) getAttributes();
  return mLevel;
}

#endif

