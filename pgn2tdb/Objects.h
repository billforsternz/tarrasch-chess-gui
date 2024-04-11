/****************************************************************************
 * A collection of important subsystems, i.e. a box of objects
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef OBJECTS_H
#define OBJECTS_H

// The object box has pointers to objects, rather than objects themselves,
//  so we can use forward references rather than including all the objects
class  GameLogic;
class  PanelContext;
class  CentralWorkSaver;
class  wxListCtrl;
class  PopupControl;
class  UciInterface;
class  wxFrame;
class  Repository;
class  Book;
class  Log;
class  Database;
class  Session;
class  Tabs;

class Objects
{
public:
    GameLogic    *gl;
    Tabs         *tabs;
    UciInterface        *uci_interface;
    wxFrame      *frame;
    PanelContext *canvas;
    Repository   *repository;
    Book         *book;
    Log          *log;
    Session      *session;
    Database     *db;
    CentralWorkSaver *cws;
};

// Singleton
extern Objects objs;

#endif // OBJECTS_H