/***************************************************************************
                          HpiView.cpp  -  description
                             -------------------
    begin                : Mon Dec  8 15:36:21 CET 2003
    copyright            : (C) 2003 by Thomas Kanngieser
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qaccel.h>
#include "HpiView.h"


cHpiViewApp *theApp = 0;


cHpiViewApp::cHpiViewApp()
{
  theApp = this;

  setCaption(tr("HpiView " VERSION));

  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initActions();
  initMenuBar();
  initToolBar();
  initStatusBar();

  initView();

  viewToolBar->setOn(true);
  viewStatusBar->setOn(true);
}


cHpiViewApp::~cHpiViewApp()
{
}


/** initializes all QActions of the application */
void
cHpiViewApp::initActions()
{
  fileQuit = new QAction(tr("Exit"), tr("E&xit"), QAccel::stringToKey(tr("Ctrl+Q")), this);
  fileQuit->setStatusTip(tr("Quits the application"));
  fileQuit->setWhatsThis(tr("Exit\n\nQuits the application"));
  connect(fileQuit, SIGNAL(activated()), this, SLOT(slotFileQuit()));

  editCut = new QAction(tr("Cut"), tr("Cu&t"), QAccel::stringToKey(tr("Ctrl+X")), this);
  editCut->setStatusTip(tr("Cuts the selected section and puts it to the clipboard"));
  editCut->setWhatsThis(tr("Cut\n\nCuts the selected section and puts it to the clipboard"));
  connect(editCut, SIGNAL(activated()), this, SLOT(slotEditCut()));

  editCopy = new QAction(tr("Copy"), tr("&Copy"), QAccel::stringToKey(tr("Ctrl+C")), this);
  editCopy->setStatusTip(tr("Copies the selected section to the clipboard"));
  editCopy->setWhatsThis(tr("Copy\n\nCopies the selected section to the clipboard"));
  connect(editCopy, SIGNAL(activated()), this, SLOT(slotEditCopy()));

  editPaste = new QAction(tr("Paste"), tr("&Paste"), QAccel::stringToKey(tr("Ctrl+V")), this);
  editPaste->setStatusTip(tr("Pastes the clipboard contents to actual position"));
  editPaste->setWhatsThis(tr("Paste\n\nPastes the clipboard contents to actual position"));
  connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));

  viewToolBar = new QAction(tr("Toolbar"), tr("Tool&bar"), 0, this, 0, true);
  viewToolBar->setStatusTip(tr("Enables/disables the toolbar"));
  viewToolBar->setWhatsThis(tr("Toolbar\n\nEnables/disables the toolbar"));
  connect(viewToolBar, SIGNAL(toggled(bool)), this, SLOT(slotViewToolBar(bool)));

  viewStatusBar = new QAction(tr("Statusbar"), tr("&Statusbar"), 0, this, 0, true);
  viewStatusBar->setStatusTip(tr("Enables/disables the statusbar"));
  viewStatusBar->setWhatsThis(tr("Statusbar\n\nEnables/disables the statusbar"));
  connect(viewStatusBar, SIGNAL(toggled(bool)), this, SLOT(slotViewStatusBar(bool)));

  helpAboutApp = new QAction(tr("About"), tr("&About..."), 0, this);
  helpAboutApp->setStatusTip(tr("About the application"));
  helpAboutApp->setWhatsThis(tr("About\n\nAbout the application"));
  connect(helpAboutApp, SIGNAL(activated()), this, SLOT(slotHelpAbout()));
}


void 
cHpiViewApp::initMenuBar()
{
  ///////////////////////////////////////////////////////////////////
  // MENUBAR

  ///////////////////////////////////////////////////////////////////
  // menuBar entry fileMenu
  fileMenu=new QPopupMenu();
  fileQuit->addTo(fileMenu);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry editMenu
  editMenu=new QPopupMenu();
  editCut->addTo(editMenu);
  editCopy->addTo(editMenu);
  editPaste->addTo(editMenu);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry viewMenu
  viewMenu=new QPopupMenu();
  viewMenu->setCheckable(true);
  viewToolBar->addTo(viewMenu);
  viewStatusBar->addTo(viewMenu);
  ///////////////////////////////////////////////////////////////////
  // EDIT YOUR APPLICATION SPECIFIC MENUENTRIES HERE

  ///////////////////////////////////////////////////////////////////
  // menuBar entry helpMenu
  helpMenu=new QPopupMenu();
  helpAboutApp->addTo(helpMenu);

  ///////////////////////////////////////////////////////////////////
  // MENUBAR CONFIGURATION
  menuBar()->insertItem(tr("&File"), fileMenu);
  menuBar()->insertItem(tr("&Edit"), editMenu);
  menuBar()->insertItem(tr("&View"), viewMenu);
  menuBar()->insertSeparator();
  menuBar()->insertItem(tr("&Help"), helpMenu);
}


void 
cHpiViewApp::initToolBar()
{
  ///////////////////////////////////////////////////////////////////
  // TOOLBAR
  fileToolbar = new QToolBar(this, "file operations");
  //  fileNew->addTo(fileToolbar);
  fileToolbar->addSeparator();
  QWhatsThis::whatsThisButton(fileToolbar);

}


void 
cHpiViewApp::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  //STATUSBAR
  statusBar()->message(tr("Ready."), 2000);
}


void
cHpiViewApp::initView()
{
  ////////////////////////////////////////////////////////////////////
  // set the main widget here
  QSplitter *s1 = new QSplitter( Qt::Vertical, this, "main" );
  QSplitter *s2 = new QSplitter( Qt::Horizontal, s1, "top" );
  
  m_log = new QMultiLineEdit( s1, "log" );
  m_log->setReadOnly( true );
 
  Log( "HpiView 0.1\n" );

  // First step in HPI and openhpi
  SaErrorT err = saHpiInitialize( &m_version );

  if ( err != SA_OK  )
     {
       QString str( "saHpiInitialize: " );
       str += hpiError2String( err );

       Log ( str );
     }

  // Every domain requires a new session
  // This example is for one domain, one session
  err = saHpiSessionOpen( SAHPI_DEFAULT_DOMAIN_ID, &m_session_id, 0 );

  if ( err != SA_OK )
     {
       LogError( "saHpiSessionOpen: ", err );
       return;
     }
  
  printf( "session open: %d.\n", m_session_id );

  err = saHpiSubscribe( m_session_id, true );

  if ( err != SA_OK )
     {
       LogError( "saHpiSubscribe: ", err );
       return;
     }

  m_resource = new cHpiViewResource( s2, "view" );

  m_data = new QTable( 0, 0, s2 );
  m_data->setReadOnly( true );

  setCentralWidget( s1 );
}


void
cHpiViewApp::Log( const char *str )
{
  m_log->append( str );
}


void
cHpiViewApp::LogError( const char *str, SaErrorT err )
{
  QString s( str );
  s += hpiError2String( err );

  Log( s );
}


bool 
cHpiViewApp::queryExit()
{
  int exit = QMessageBox::information( this, tr("Quit..."),
                                      tr("Do your really want to quit?"),
                                      QMessageBox::Ok, QMessageBox::Cancel);

  if ( exit==1 )
     {
     }
  else
     {
     };

  return ( exit == 1 );
}


void 
cHpiViewApp::slotFileQuit()
{
  statusBar()->message(tr("Exiting application..."));

  saHpiUnsubscribe( m_session_id );
  saHpiSessionClose( m_session_id );
  saHpiFinalize();

  qApp->quit();

  statusBar()->message(tr("Ready."));
}


void
cHpiViewApp::slotEditCut()
{
  statusBar()->message(tr("Cutting selection..."));
  statusBar()->message(tr("Ready."));
}


void 
cHpiViewApp::slotEditCopy()
{
  statusBar()->message(tr("Copying selection to clipboard..."));
  statusBar()->message(tr("Ready."));
}


void
cHpiViewApp::slotEditPaste()
{
  statusBar()->message(tr("Inserting clipboard contents..."));
  statusBar()->message(tr("Ready."));
}


void
cHpiViewApp::slotViewToolBar(bool toggle)
{
  statusBar()->message(tr("Toggle toolbar..."));
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off

  if ( toggle == false )
       fileToolbar->hide();
  else
       fileToolbar->show();

  statusBar()->message(tr("Ready."));
}


void 
cHpiViewApp::slotViewStatusBar( bool toggle )
{
  statusBar()->message(tr("Toggle statusbar..."));
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off

  if ( toggle == false )
       statusBar()->hide();
  else
       statusBar()->show();

  statusBar()->message(tr("Ready."));
}


void
cHpiViewApp::slotHelpAbout()
{
  QMessageBox::about(this,tr("About..."),
                     tr("HpiView\nVersion " VERSION "\n(c) 2003 by Thomas Kanngieser") );
}
