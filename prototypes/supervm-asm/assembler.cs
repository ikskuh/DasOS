using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace supervm_asm
{
	class Program
	{


		static void Main(string[] args)
		{
			// MnemonicParser.GenerateFromDocumentation(@"../supervm/supervm.md");

			foreach(var file in args.Where(a => !a.StartsWith("-") && Path.GetExtension(a) == ".asm"))
			{
				var output = Path.ChangeExtension(file, ".bin");
				var code = Assembler.Assemble(File.ReadAllText(file));
				
				Console.WriteLine("{0}:", output);
				for (int i = 0; i < code.Length; i++)
				{
					Console.Write("; {0:X3} ", i);
					PrintInstruction(code[i]);
				}
				using(var fs = File.Open(output, FileMode.Create, FileAccess.Write))
				{
					for(int i = 0; i < code.Length; i++)
					{
						var bits = BitConverter.GetBytes(code[i]);
						if(BitConverter.IsLittleEndian == false)
						{
							bits = bits.Reverse().ToArray();
						}
						fs.Write(bits, 0, bits.Length);
					}
				}
			}
		}
		
		static void PrintInstruction(ulong instr)
		{
			var str = Convert.ToString((long)instr, 2).PadLeft(64, '0');
			
			var portions = new []
			{
				new { Start = 0,  Length = 32, Color = ConsoleColor.Red },
				new { Start = 32, Length = 2,  Color = ConsoleColor.DarkGreen },
				new { Start = 34, Length = 1,  Color = ConsoleColor.Green },
				new { Start = 35, Length = 16, Color = ConsoleColor.Magenta },
				new { Start = 51, Length = 6,  Color = ConsoleColor.Yellow },
				new { Start = 57, Length = 1,  Color = ConsoleColor.DarkCyan },
				new { Start = 58, Length = 2,  Color = ConsoleColor.Cyan },
				new { Start = 60, Length = 2,  Color = ConsoleColor.DarkBlue },
				new { Start = 62, Length = 2,  Color = ConsoleColor.Blue },
			};
			
			var fg = Console.ForegroundColor;
			foreach(var portion in portions)
			{
				Console.ForegroundColor = portion.Color;
				Console.Write("{0} ", str.Substring(portion.Start, portion.Length));
			}
			Console.ForegroundColor = fg;
			Console.WriteLine();
		}
	}

	public static class Assembler
	{
		static Regex annotationMatcher = new Regex(@"\[\s*(.*?)\s*\]", RegexOptions.Compiled);
		static Regex labelMatcher = new Regex(@"^(\w+):\s*(.*)\s*$", RegexOptions.Compiled);
		static Regex instructionMatcher = new Regex(@"(\w+)(?:\s+(@?\w+))?", RegexOptions.Compiled);

		public static ulong[] Assemble(string src)
		{
			var lines = src.Split(new[] { '\n', '\r' }, StringSplitOptions.RemoveEmptyEntries);
			var patches = new Dictionary<int, string>();
			var labels = new Dictionary<string, int>();

			var code = new List<ulong>();
			for (int i = 0; i < lines.Length; i++)
			{
				var line = lines[i].Trim();
				{ // Process comments
					var idx = line.IndexOf(';');
					if (idx >= 0)
						line = line.Substring(0, idx);
				}

				{ // Process labels
					var match = labelMatcher.Match(line);
					if (match.Success)
					{
						var label = match.Groups[1].Value;
						labels.Add(label, code.Count);
						line = match.Groups[2].Value;
					}
				}

				if (string.IsNullOrWhiteSpace(line))
					continue;

				var annotations = new HashSet<string>();
				line = annotationMatcher.Replace(line, (m) =>
				{
					annotations.Add(m.Groups[1].Value.ToLower());
					return "";
				});
				line = line.Trim();

				{
					var match = instructionMatcher.Match(line);
					if (match.Success == false)
						throw new InvalidOperationException("Invalid instruction: " + line);

					var mnemonic = match.Groups[1].Value;

					uint argument = 0;
					if (match.Groups[2].Length > 0)
					{
						var argstring = match.Groups[2].Value;
						if (argstring.StartsWith("@"))
						{
							// Add patch note for labels.
							patches.Add(code.Count, argstring.Substring(1));
						}
						else if (argstring.StartsWith("0x"))
						{
							argument = Convert.ToUInt32(argstring.Substring(2), 16);
						}
						else if (argstring.StartsWith("0b"))
						{
							argument = Convert.ToUInt32(argstring.Substring(2), 10);
						}
						else if (argstring.StartsWith("0d"))
						{
							argument = Convert.ToUInt32(argstring.Substring(2), 10);
						}
						else
						{
							argument = Convert.ToUInt32(argstring, 10);
						}
					}

					var instruction = mnemonics[mnemonic];

					foreach(var annotation in annotations)
					{
						if(annotation.StartsWith("ci:"))
						{
							instruction.CommandInfo = UInt16.Parse(annotation.Substring(3));
							continue;
						}

						if (annotation.StartsWith("cmd:"))
						{
							instruction.Command = (Command)Enum.Parse(typeof(Command), annotation.Substring(4));
							continue;
						}

						switch (annotation)
						{
							case "f:yes":
								instruction.ModifyFlags = true;
								break;
							case "f:no":
								instruction.ModifyFlags = false;
								break;
							case "r:discard":
								instruction.Output = OutputType.Discard;
								break;
							case "r:push":
								instruction.Output = OutputType.Push;
								break;
							case "r:jump":
								instruction.Output = OutputType.Jump;
								break;
							case "i0:zero":
								instruction.Input0 = InputType.Zero;
								break;
							case "i0:pop":
								instruction.Input0 = InputType.Pop;
								break;
							case "i0:peek":
								instruction.Input0 = InputType.Peek;
								break;
							case "i0:arg":
								instruction.Input0 = InputType.Argument;
								break;
							case "i1:zero":
								instruction.Input1 = InputType.Zero;
								break;
							case "i1:pop":
								instruction.Input1 = InputType.Pop;
								break;
							case "ex(z)=x":
								instruction.ExecutionZ = ExecutionMode.Always;
								break;
							case "ex(z)=0":
								instruction.ExecutionZ = ExecutionMode.Zero;
								break;
							case "ex(z)=1":
								instruction.ExecutionZ = ExecutionMode.One;
								break;
							case "ex(n)=x":
								instruction.ExecutionN = ExecutionMode.Always;
								break;
							case "ex(n)=0":
								instruction.ExecutionN = ExecutionMode.Zero;
								break;
							case "ex(n)=1":
								instruction.ExecutionN = ExecutionMode.One;
								break;
							default:
								throw new InvalidOperationException("Unrecognized annotation: " + annotation);
						}
					}

					ulong encoded = 0;

					encoded |= ((uint)(instruction.ExecutionZ) << 0);
					encoded |= ((uint)(instruction.ExecutionN) << 2);
					encoded |= ((uint)(instruction.Input0) << 4);
					encoded |= ((uint)(instruction.Input1) << 6);
					encoded |= ((uint)(instruction.Command) << 7);
					encoded |= ((uint)(instruction.CommandInfo) << 13);
					encoded |= ((uint)(instruction.ModifyFlags ? 1 : 0) << 29);
					encoded |= ((uint)(instruction.Output) << 30);
					encoded |= ((ulong)argument << 32);
			
					code.Add(encoded);
				}
			}

			{ // Install patches
				foreach (var patch in patches)
				{
					var target = patch.Value;
					var position = labels[target];
					code[patch.Key] =
						(code[patch.Key] & 0xFFFFFFFF) |
						((ulong)position << 32);
				}
			}
			
			return code.ToArray();
		}


		static Dictionary<string, Instruction> mnemonics = new Dictionary<string, Instruction>()
		{
			{ "nop", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Zero, Input1 = InputType.Zero, Command = Command.Copy, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Discard, Argument = 0,  } },
			{ "push", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Argument, Input1 = InputType.Zero, Command = Command.Copy, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "drop", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Zero, Command = Command.Copy, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Discard, Argument = 0,  } },
			{ "dup", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Peek, Input1 = InputType.Zero, Command = Command.Copy, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "jmp", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Argument, Input1 = InputType.Zero, Command = Command.Copy, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Jump, Argument = 0,  } },
			{ "jmpi", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Zero, Command = Command.Copy, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Jump, Argument = 0,  } },
			{ "ret", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Zero, Command = Command.Copy, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Jump, Argument = 0,  } },
			{ "load", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Argument, Input1 = InputType.Zero, Command = Command.Load, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "loadi", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Zero, Command = Command.Load, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "store", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Argument, Input1 = InputType.Pop, Command = Command.Store, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Discard, Argument = 0,  } },
			{ "storei", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Store, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Discard, Argument = 0,  } },
			{ "get", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Argument, Input1 = InputType.Zero, Command = Command.Get, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "geti", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Zero, Command = Command.Get, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "set", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Argument, Input1 = InputType.Pop, Command = Command.Set, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Discard, Argument = 0,  } },
			{ "seti", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Set, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Discard, Argument = 0,  } },
			{ "bpget", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Zero, Input1 = InputType.Zero, Command = Command.BpGet, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "bpset", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Zero, Command = Command.BpSet, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Discard, Argument = 0,  } },
			{ "add", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "sub", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 1, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "cmp", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 1, ModifyFlags = true, Output = OutputType.Discard, Argument = 0,  } },
			{ "mul", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 2, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "div", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 3, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "mod", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 4, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "and", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 5, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "or", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 6, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "xor", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 7, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "not", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Zero, Command = Command.Math, CommandInfo = 8, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "rol", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 9, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "ror", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 10, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "asl", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 11, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "asr", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 12, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "shl", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 13, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "shr", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Pop, Input1 = InputType.Pop, Command = Command.Math, CommandInfo = 14, ModifyFlags = false, Output = OutputType.Push, Argument = 0,  } },
			{ "syscall", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Zero, Input1 = InputType.Zero, Command = Command.SysCall, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Discard, Argument = 0,  } },
			{ "hwio", new Instruction() { ExecutionZ = ExecutionMode.Always, ExecutionN = ExecutionMode.Always, Input0 = InputType.Zero, Input1 = InputType.Zero, Command = Command.HwIO, CommandInfo = 0, ModifyFlags = false, Output = OutputType.Discard, Argument = 0,  } },
		};
	}

	class MnemonicParser
	{
		public static void GenerateFromDocumentation(string file)
		{
			var relevantLines = File
				.ReadAllLines(file)
				.SkipWhile(l => l != "## Assembler Mnemonics")
				.Skip(1)
				.SkipWhile(l => string.IsNullOrWhiteSpace(l))
				.TakeWhile(l => l.StartsWith("|"))
				.Skip(2)
				.ToArray();



			var instructions = relevantLines
				.Select(l => l
					.Split(new[] { '|' }, StringSplitOptions.RemoveEmptyEntries)
					.Select(s => s.Trim())
					.ToArray())
				.ToDictionary(a => a[0], a => new Instruction()
				{
					ExecutionN = ExecutionMode.Always,
					ExecutionZ = ExecutionMode.Always,
					Input0 = ToInputMode(a[2]),
					Input1 = ToInputMode(a[3]),
					Command = ToCommand(a[4]),
					CommandInfo = ushort.Parse(a[5]),
					ModifyFlags = (a[7].ToLower() == "yes"),
					Output = ToOutput(a[6]),
					Argument = 0,
				});

			var fields = typeof(Instruction).GetFields();
			foreach (var item in instructions)
			{
				// { "a", new Instruction() {  } },
				Console.Write("{{ \"{0}\", new Instruction() {{ ", item.Key);

				foreach (var field in fields)
				{
					var value = field.GetValue(item.Value);
					if (field.FieldType.IsEnum)
						Console.Write("{0} = {1}.{2}, ", field.Name, field.FieldType.Name, value);
					else
						Console.Write("{0} = {1}, ", field.Name, value.ToString().ToLower());
				}

				Console.WriteLine(" } },");
			}
		}

		private static OutputType ToOutput(string v)
		{
			switch (v.ToLower())
			{
				case "discard": return OutputType.Discard;
				case "push": return OutputType.Push;
				case "jump": return OutputType.Jump;
				default:
					throw new NotSupportedException();
			}
		}

		private static Command ToCommand(string v)
		{
			return (Command)Enum.Parse(typeof(Command), v, true);
		}

		private static InputType ToInputMode(string v)
		{
			switch (v.ToLower())
			{
				case "zero": return InputType.Zero;
				case "peek": return InputType.Peek;
				case "pop": return InputType.Pop;
				case "arg": return InputType.Argument;
				default:
					throw new NotSupportedException();
			}
		}
	}

	public struct Instruction
	{
		public ExecutionMode ExecutionZ;
		public ExecutionMode ExecutionN;
		public InputType Input0;
		public InputType Input1;
		public Command Command;
		public ushort CommandInfo;
		public bool ModifyFlags;
		public OutputType Output;
		public uint Argument;
	}

	public enum MathCommand
	{
		Add = 0,
		Subtract = 1,
		Multiplicate = 2,
		Divide = 3,
		Modulo = 4,
		And = 5,
		Or = 6,
		Xor = 7,
		Not = 8,
		RotShiftLeft = 9,
		RotShiftRight = 10,
		ArithmeticShiftLeft = 11,
		ArithmeticShiftRight = 12,
		LogicShiftLeft = 13,
		LogicShiftRight = 14,
	}

	public enum Command
	{
		Copy = 0,
		Store = 1,
		Load = 2,
		Get = 3,
		Set = 4,
		BpGet = 5,
		BpSet = 6,
		RstStack = 7,
		Math = 8,
		SpGet = 9,
		SpSet = 10,
		SysCall = 11,
		HwIO = 12,
	}

	public enum OutputType
	{
		Discard = 0,
		Push = 1,
		Jump = 2,
	}

	public enum ExecutionMode
	{
		Always = 0,
		Zero = 2,
		One = 3,
	}

	public enum InputType
	{
		Zero = 0,
		Pop = 1,
		Peek = 2,
		Argument = 3,
	}
}
