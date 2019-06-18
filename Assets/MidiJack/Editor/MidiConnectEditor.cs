using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.Reflection;
using System;

namespace MidiJack {
    [CustomEditor(typeof(MonoBehaviour), true)]
    public class MonoBehaviourEditor : Editor
    {
        IDictionary<int, List<FieldInfo>> attributes;
        bool dirty;
        void OnEnable() {
            attributes = new Dictionary<int, List<FieldInfo>>();
            MonoBehaviour mono = target as MonoBehaviour;
            FieldInfo[] objectFields = mono.GetType().GetFields(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);

            for (int i = 0; i < objectFields.Length; i++) {
                MidiConnect attribute = objectFields[i].GetCustomAttribute(typeof(MidiConnect),true) as MidiConnect;
                if (attribute != null) {
                    if(!attributes.ContainsKey(attribute.knobId)) {
                        attributes.Add(attribute.knobId, new List<FieldInfo>());
                    }
                    attributes[attribute.knobId].Add(objectFields[i]);
                }
            }
            
            MidiMaster.noteOnDelegate += NoteOnEditor;
            MidiMaster.noteOffDelegate += NoteOffEditor;
            MidiMaster.knobDelegate += KnobEditor;
        }

        void OnDisable()
        {
            MidiMaster.noteOnDelegate -= NoteOnEditor;
            MidiMaster.noteOffDelegate -= NoteOffEditor;
            MidiMaster.knobDelegate -= KnobEditor;
        }
        
        public override void OnInspectorGUI() {
            base.OnInspectorGUI();
            var msg = MidiDriver.Instance.TotalMessageCount;
            EditorUtility.SetDirty(target);
        }

        void NoteOnEditor(MidiChannel channel, int note, float velocity) {
            if(!attributes.ContainsKey(note)) {
                return;
            }
            foreach(FieldInfo field in attributes[note]) {
                if(field.FieldType == typeof(bool) || field.FieldType == typeof(Boolean)){
                    field.SetValue((MonoBehaviour)target, true);
                } else {
                    field.SetValue((MonoBehaviour)target, velocity);
                }
            }
        }

        void NoteOffEditor(MidiChannel channel, int note) {
            if(!attributes.ContainsKey(note)) {
                return;
            }
            foreach(FieldInfo field in attributes[note]) {
                if(field.FieldType == typeof(bool) || field.FieldType == typeof(Boolean)){
                    field.SetValue((MonoBehaviour)target, false);
                } else {
                    field.SetValue((MonoBehaviour)target, 0);
                }
            }
        }

        void KnobEditor(MidiChannel channel, int knobNumber, float knobValue) {
            if(!attributes.ContainsKey(knobNumber)) {
                return;
            }
            foreach(FieldInfo field in attributes[knobNumber]) {
                if(field.FieldType == typeof(bool) || field.FieldType == typeof(Boolean)){
                    field.SetValue((MonoBehaviour)target, knobValue > 0f ? true : false);
                } else {
                    field.SetValue((MonoBehaviour)target, knobValue);
                }
            }
        }
    }
}