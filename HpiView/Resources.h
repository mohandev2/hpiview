/***************************************************************************
                          Resources.h  -  description
                             -------------------
    begin                : Mon Dec  8 15:36:21 CET 2003
    copyright            : (C) 2003 by FORCE Computers
    email                : Thomas.Kanngieser@fci.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef RESOURCES_H
#define RESOURCES_H


#include "Data.h"
#include <qlistview.h>
#include <qtimer.h>


class cHpiItem : public QListViewItem
{
  char m_name[80];
  bool m_mark;

public:
  cHpiItem( QListView *parent, const char *name );
  cHpiItem( cHpiItem *parent, const char *name );

  cHpiItem *findChild( const char *name );

  virtual void activate();

  void mark( bool m = true, bool rec = false );

  void deleteUnmark();

  bool isName( const char *name )
  {
    return !strcmp( m_name, name );
  }
};


class cHpiResourceRoot : public cHpiItem
{
  cHpiItem *createEntityPath( const SaHpiEntityPathT &path, int max = 1 );

public:
  cHpiResourceRoot( QListView *parent, const char *name );

  void discoverResources();
};


class cHpiResourceEntity : public cHpiItem
{
public:
  SaHpiEntityT m_entity;

  cHpiResourceEntity( cHpiItem *parent, const char *name, 
                      const SaHpiEntityT &ent )
    : cHpiItem( parent, name ), m_entity( ent )
  {
    setText( 1, "Entity" );
  }
};


class cHpiResourceRptItem : public cHpiResourceEntity
{
public:
  SaHpiRptEntryT m_rpt_entry;

  cHpiResourceRptItem( cHpiItem *parent, const char *name,
                       const SaHpiEntityT &ent, const SaHpiRptEntryT &rpt_entry )
    : cHpiResourceEntity( parent, name, ent ), m_rpt_entry( rpt_entry )
  {
    setText( 1, "RPT Entry" );
  }

  virtual void activate();
};


class cHpiResourceRdr : public cHpiItem
{
public:
  SaHpiRdrT m_rdr;

  cHpiResourceRdr( cHpiItem *parent, const char *name,
                   const SaHpiRdrT &rdr )
    : cHpiItem( parent, name ), m_rdr( rdr )
  {
    setText( 1, "RDR" );
  }

  virtual void activate();
};


#define dMaxInventorySize 10240

class cHpiResourceRdrInventory : public cHpiResourceRdr
{
public:
  unsigned char m_inventory[dMaxInventorySize];
  unsigned int m_size;
  
  cHpiResourceRdrInventory( cHpiItem *parent, const char *name,
                   const SaHpiRdrT &rdr )
    : cHpiResourceRdr( parent, name, rdr )
  {
    setText( 1, "RDR Inventory" );
  }
};


class cHpiResourceInventoryInternalUse : public cHpiItem
{
public:
  SaHpiInventInternalUseDataT *m_internal;

  cHpiResourceInventoryInternalUse( cHpiItem *parent, const char *name,
                                    SaHpiInventInternalUseDataT *iu )
    : cHpiItem( parent, name ), m_internal( iu )
  {
    setText( 1, "Internal Use" );
    mark( true );
  }

  virtual void activate();
};


class cHpiResourceInventoryChassis : public cHpiItem
{
public:
  SaHpiInventChassisDataT *m_chassis;

  cHpiResourceInventoryChassis( cHpiItem *parent, const char *name,
                                SaHpiInventChassisDataT *ch )
    : cHpiItem( parent, name ), m_chassis( ch )
  {
    setText( 1, "Chassis" );
    mark( true );
  }

  virtual void activate();
};


class cHpiResourceInventoryBoard : public cHpiItem
{
public:
  SaHpiInventGeneralDataT *m_board;

  cHpiResourceInventoryBoard( cHpiItem *parent, const char *name,
                              SaHpiInventGeneralDataT *board )
    : cHpiItem( parent, name ), m_board( board )
  {
    setText( 1, "Board" );
    mark( true );
  }

  virtual void activate();
};


class cHpiResourceInventoryProduct : public cHpiItem
{
public:
  SaHpiInventGeneralDataT *m_product;

  cHpiResourceInventoryProduct( cHpiItem *parent, const char *name,
                                SaHpiInventGeneralDataT *pr )
    : cHpiItem( parent, name ), m_product( pr )
  {
    setText( 1, "Product" );
    mark( true );
  }

  virtual void activate();
};


class cHpiResourceInventoryOem : public cHpiItem
{
public:
  SaHpiInventOemDataT *m_oem;

  cHpiResourceInventoryOem( cHpiItem *parent, const char *name,
                            SaHpiInventOemDataT *oem )
    : cHpiItem( parent, name ), m_oem( oem )
  {
    setText( 1, "Oem" );
    mark( true );
  }

  virtual void activate();
};


class cHpiEventRoot : public cHpiItem
{
  int m_current;

  bool getEvent();

public:
  cHpiEventRoot( QListView *parent, const char *name )
    : cHpiItem( parent, name ), m_current( 0 )
  {
  }

  void getEvents();
};


class cHpiEvent : public cHpiItem
{
  SaHpiEventT    m_event;
  SaHpiRdrT      m_rdr;
  SaHpiRptEntryT m_rpt_entry;

public:
  cHpiEvent( cHpiItem *parent, const char *name,
             const SaHpiEventT &event, const SaHpiRdrT &rdr,
             const SaHpiRptEntryT &rpt_entry )
    : cHpiItem( parent, name ), m_event( event ),
      m_rdr( rdr ), m_rpt_entry( rpt_entry )
  {
  }

  bool WithRdr()
  {
    return m_rdr.RdrType != SAHPI_NO_RECORD;
  }

  bool WithRptEntry()
  {  
    return m_rpt_entry.ResourceCapabilities;
  }
  
  virtual void activate();
};


class cHpiViewResource : public QListView
{
  Q_OBJECT

  cHpiResourceRoot *m_resource;
  QTimer           *m_resource_timer;

  cHpiEventRoot    *m_event;
  QTimer           *m_event_timer;

public slots:
  void resourceTimeout(); 
  void eventTimeout(); 

public:
  cHpiViewResource( QWidget*parent, const char* name );
};


#endif
