=begin
  dndtreeview.rb - Drag and Drop sample script.

  Copyright (C) 2003-2005 Masao Mutoh
  This program is licenced under the same licence as Ruby-GNOME2.

  $Date: 2005/07/14 17:01:49 $
  $Id: dndtreeview.rb,v 1.2 2005/07/14 17:01:49 mutoh Exp $
=end

require 'gtk2'

Gtk.init

class TestWindow < Gtk::Window
  attr_reader :list

  TARGET_TABLE = [["GTK_TREE_MODEL_ROW", 0, 0]]

  def initialize()
    super("Drag and Drop Gtk::TreeView")

    set_default_size(300,128)
    signal_connect("destroy") { Gtk.main_quit }

    data = [["Hello", "KON-NI-CHIWA"],
      ["Goodbye", "SAYO-NARA"],
      ["Good morning", "OHA-YO-GOZAI-MASU"],
      ["Good evening", "KON-BAN-WA"]]

    model = Gtk::ListStore.new(String, String)
    view = Gtk::TreeView.new(model)
    renderer = Gtk::CellRendererText.new
    col1 = Gtk::TreeViewColumn.new("Data", renderer, {:text => 0})
    col2 = Gtk::TreeViewColumn.new("Data", renderer, {:text => 1})
    view.append_column(col1)
    view.append_column(col2)

    data.each do |v1, v2|
      iter = model.append
      iter.set_value(0, v1)
      iter.set_value(1, v2)
    end

    view.enable_model_drag_source(Gdk::Window::BUTTON1_MASK,
                                  TARGET_TABLE, 
                                  Gdk::DragContext::ACTION_COPY|Gdk::DragContext::ACTION_MOVE)
    view.enable_model_drag_dest(TARGET_TABLE,
                                 Gdk::DragContext::ACTION_COPY|Gdk::DragContext::ACTION_MOVE) 

    add(view)
  end
end

win = TestWindow.new.show_all

Gtk.main

