/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-09 Ben Fry and Casey Reas
  Copyright (c) 2001-04 Massachusetts Institute of Technology

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

package processing.app;

import java.io.*;
import java.util.*;


/**
 * Storage class for user preferences and environment settings.
 * <P>
 * This class no longer uses the Properties class, since
 * properties files are iso8859-1, which is highly likely to
 * be a problem when trying to save sketch folders and locations.
 * <p>
 * The GUI portion in here is really ugly, as it uses exact layout. This was
 * done in frustration one evening (and pre-Swing), but that's long since past,
 * and it should all be moved to a proper swing layout like BoxLayout.
 * <p>
 * This is very poorly put together, that the preferences panel and the actual
 * preferences i/o is part of the same code. But there hasn't yet been a
 * compelling reason to bother with the separation aside from concern about
 * being lectured by strangers who feel that it doesn't look like what they
 * learned in CS class.
 * <p>
 * Would also be possible to change this to use the Java Preferences API.
 * Some useful articles
 * <a href="http://www.onjava.com/pub/a/onjava/synd/2001/10/17/j2se.html">here</a> and
 * <a href="http://www.particle.kth.se/~lindsey/JavaCourse/Book/Part1/Java/Chapter10/Preferences.html">here</a>.
 * However, haven't implemented this yet for lack of time, but more
 * importantly, because it would entail writing to the registry (on Windows),
 * or an obscure file location (on Mac OS X) and make it far more difficult to
 * find the preferences to tweak them by hand (no! stay out of regedit!)
 * or to reset the preferences by simply deleting the preferences.txt file.
 */
public class Preferences {

  // what to call the feller

  static final String PREFS_FILE = "preferences.txt";


  // prompt text stuff

  static final String PROMPT_YES     = "Yes";
  static final String PROMPT_NO      = "No";
  static final String PROMPT_CANCEL  = "Cancel";
  static final String PROMPT_OK      = "OK";
  static final String PROMPT_BROWSE  = "Browse";

  /**
   * Standardized width for buttons. Mac OS X 10.3 wants 70 as its default,
   * Windows XP needs 66, and my Ubuntu machine needs 80+, so 80 seems proper.
   */
  static public int BUTTON_WIDTH  = 80;

  /**
   * Standardized button height. Mac OS X 10.3 (Java 1.4) wants 29,
   * presumably because it now includes the blue border, where it didn't
   * in Java 1.3. Windows XP only wants 23 (not sure what default Linux
   * would be). Because of the disparity, on Mac OS X, it will be set
   * inside a static block.
   */
  static public int BUTTON_HEIGHT = 24;

  // value for the size bars, buttons, etc

  static final int GRID_SIZE     = 33;


  // indents and spacing standards. these probably need to be modified
  // per platform as well, since macosx is so huge, windows is smaller,
  // and linux is all over the map

  static final int GUI_BIG     = 13;
  static final int GUI_BETWEEN = 10;
  static final int GUI_SMALL   = 6;



  // data model

  static Hashtable table = new Hashtable();;
  static File preferencesFile;


  static protected void init(String commandLinePrefs) {


  }


  public Preferences() {

  }

  // .................................................................

  // .................................................................

  // .................................................................

  // .................................................................



  static public String get(String attribute) {
    return (String) table.get(attribute);
  }

  static public void set(String attribute, String value) {
    table.put(attribute, value);
  }


  static public boolean getBoolean(String attribute) {
    String value = get(attribute);
    return (new Boolean(value)).booleanValue();
  }


  static public void setBoolean(String attribute, boolean value) {
    set(attribute, value ? "true" : "false");
  }


  static public int getInteger(String attribute) {
    return Integer.parseInt(get(attribute));
  }


  static public void setInteger(String key, int value) {
    set(key, String.valueOf(value));
  }

}
