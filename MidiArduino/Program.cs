using System;
using System.Collections.Generic;
using System.IO;
using NAudio.Midi;

namespace MidiArduino
{
    class Program
    {
        static void Main(string[] args)
        {
            //Ask the user for the path to the MIDI file
            Console.WriteLine("Enter the path to the MIDI file:");
            string path = Console.ReadLine();

            var midiFile = new MidiFile(path, false);

            List<int> frequencies = new List<int>();
            List<int> durations = new List<int>();

            int ticksPerQuarterNote = midiFile.DeltaTicksPerQuarterNote;

            foreach (var track in midiFile.Events)
            {
                Dictionary<int, int> noteStartTimes = new Dictionary<int, int>();

                foreach (var midiEvent in track)
                {
                    if (midiEvent is NoteOnEvent noteOn)
                    {
                        if (noteOn.Velocity > 0)
                        {
                            // Start of a note
                            noteStartTimes[noteOn.NoteNumber] = (int)noteOn.AbsoluteTime;
                        }
                        else
                        {
                            //Note off event (velocity 0)
                            if (noteStartTimes.ContainsKey(noteOn.NoteNumber))
                            {
                                int startTime = noteStartTimes[noteOn.NoteNumber];
                                int length = (int)noteOn.AbsoluteTime - startTime;

                                //TODO: Add Note number and duration to the lists
                                AddNote(noteOn.NoteNumber, length, ticksPerQuarterNote, frequencies, durations);

                                noteStartTimes.Remove(noteOn.NoteNumber);
                            }
                        }
                    }
                    else if (midiEvent is NoteEvent noteOff && noteOff.CommandCode == MidiCommandCode.NoteOff)
                    {
                        if (noteStartTimes.ContainsKey(noteOff.NoteNumber))
                        {
                            int start = noteStartTimes[noteOff.NoteNumber];
                            int length = (int)noteOff.AbsoluteTime - start;

                            //TODO: Add Note number and duration to the lists
                            AddNote(noteOff.NoteNumber, length, ticksPerQuarterNote, frequencies, durations);

                            noteStartTimes.Remove(noteOff.NoteNumber);
                        }
                    }
                }
            }

            // Output the frequencies and durations
            string output = "midiData.cpp";

            using (StreamWriter writer = new StreamWriter(output))
            {
                //TODO: Figure out a way later to generate it so it can be easily used in the Arduino code
                writer.WriteLine("static const uint16_t melody[] = {");
                writer.WriteLine("    " + string.Join(", ", frequencies));
                writer.WriteLine("};\n");

                writer.WriteLine("static const uint8_t durations[] = {");
                writer.WriteLine("    " + string.Join(", ", durations));
                writer.WriteLine("};\n");
            }

            Console.WriteLine($"MIDI data has been processed and saved to {output}");
        }

        static void AddNote(int midiNote, int lengthTicks, int tpq, List<int> freqs, List<int> durs)
        {
            int freq = MidiNoteToFrequency(midiNote);

            //Convert ticks to note length (quarter = 4)
            double noteLength = lengthTicks / tpq;

            int duration = ConvertToArduinoDuration(noteLength);

            freqs.Add(freq);
            durs.Add(duration);
        }

        static int MidiNoteToFrequency(int midiNote)
        {
            return (int)(440.0 * Math.Pow(2, (midiNote - 69) / 12.0));
        }

        static int ConvertToArduinoDuration(double noteLength)
        {
            //Map to the closest standard note duration (whole, half, quarter, eighth, sixteenth)
            if (noteLength >= 4)
                return 1; // Whole note
            else if (noteLength >= 2)
                return 2; // Half note
            else if (noteLength >= 1)
                return 4; // Quarter note
            else if (noteLength >= 0.5)
                return 8; // Eighth note
            else
                return 16; // Sixteenth note
        }
    }
}
