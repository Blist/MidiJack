using System;

namespace MidiJack {
  [AttributeUsage(AttributeTargets.All, 
                  Inherited = true, 
                  AllowMultiple = false)]
  public class MidiConnect : Attribute
  {
    private int _knobId = 0;
    public int knobId
    {
      get { return this._knobId; }
      set { this._knobId = value; }
    }
    
    public MidiConnect(int knobId)
    {
      this.knobId = knobId;
    }
  }
}
