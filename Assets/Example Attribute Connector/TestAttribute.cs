using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using MidiJack;

public class TestAttribute : MonoBehaviour
{
    [MidiConnect(1)]
    public float knob1;
    
    [MidiConnect(2)]
    public float knob2;
    
    [MidiConnect(61)] // Plugged on C-3 
    public bool CCChange;
}
