using System;
using System.IO;
using System.Collections.Generic;
using System.Text;

namespace fools {

	class HC4_PathGenerator {
		enum Biomes { Grass, Steppes, Construct, Corruption }

		class Tile {
			public byte value;
			public int x;
			public int y;
			public bool found;
			public Tile parent;
		}

		const int MAP_WIDTH = 0x62;
		const int MAP_HEIGHT = 0x10000 - 0xF932 - 1;
		const int FULL_WIDTH = MAP_WIDTH * 8;
		const int FULL_HEIGHT = MAP_HEIGHT * 8;

		Tile[][] fullMap;
		Biomes[][] biomeMap;

		int reviverSeeds;


		public static void _Main() {
			new HC4_PathGenerator().Run();
		}

		void Run() {
			SetUpMap();
			Tile start = fullMap[12][FULL_HEIGHT - 1];
			Tile end = fullMap[FULL_WIDTH - 133][0];
			Traverse(start, end);

			List<Tile> tiles = new List<Tile>();
			Tile onPath = end;
			tiles.Add(end);
			while (onPath != start) {
				onPath = onPath.parent;
				tiles.Add(onPath);
			}
			tiles.Reverse();

			string output = BuildPath(tiles);
			File.WriteAllText("path.txt", output);
		}


		void SetUpMap() {
			fullMap = new Tile[FULL_WIDTH][];
			for (int i = 0; i < fullMap.Length; i++) {
				fullMap[i] = new Tile[FULL_HEIGHT];
			}
			using (FileStream fs = new FileStream("hugefuckingmap.bin", FileMode.Open)) {
				int x;
				for (int y = 0; y < FULL_HEIGHT; y++) {
					for (x = 0; x < FULL_WIDTH; x++) {
						fullMap[x][y] = new Tile {
							value = (byte)fs.ReadByte(),
							x = x,
							y = y
						};
						/*
						if (y < 64 && x < 64) {
							string test = fullMap[x][y].value.ToString("x2") + " ";
							if (test != "0f ")
								Console.ForegroundColor = ConsoleColor.Green;
							else
								Console.ForegroundColor = ConsoleColor.White;
						}
						*/
					}
				}
			}
		}

		void Traverse(Tile start, Tile end) {
			Queue<Tile> searching = new Queue<Tile>();
			searching.Enqueue(start);
			start.found = true;
			while (searching.Count > 0) {
				Tile tile = searching.Dequeue();
				if (tile == end)
					return;
				if (tile.x > 0)
					Check(fullMap[tile.x - 1][tile.y]);
				if (tile.x < FULL_WIDTH - 1)
					Check(fullMap[tile.x + 1][tile.y]);
				if (tile.y > 0)
					Check(fullMap[tile.x][tile.y - 1]);
				if (tile.y < FULL_HEIGHT - 1)
					Check(fullMap[tile.x][tile.y + 1]);

				void Check(Tile t) {
					if (t.value == 0x55 && !t.found) {
						t.parent = tile;
						t.found = true;
						searching.Enqueue(t);
					}
				}
			}
		}

		string BuildPath(List<Tile> tiles) {
			StringBuilder sb = new StringBuilder();
			for (int i = 0; i < tiles.Count - 1; i++) {
				if (tiles[i].x != tiles[i + 1].x) {
					if (tiles[i + 1].x > tiles[i].x)
						sb.Append("RR");
					else
						sb.Append("LL");
					if (reviverSeeds != 131)
						sb.Append(CheckForItem(tiles[i + 1].x, tiles[i + 1].y, false));

				}
				else {
					if (tiles[i + 1].y > tiles[i].y)
						sb.Append("DD");
					else
						sb.Append("UU");
					if (reviverSeeds != 131)
						sb.Append(CheckForItem(tiles[i + 1].x, tiles[i + 1].y, true));
				}
			}
			return sb.ToString();
		}

		//up there for laziest code i've written
		string CheckForItem(int origX, int origY, bool vertical) {
			StringBuilder sb = new StringBuilder();
			int x = origX;
			int y = origY;
			if (vertical) {
				while (x > 0 && fullMap[x][origY].value != 0x0F) {
					if (fullMap[x][origY].value == 8) {
						for (int i = 0; i < Math.Abs(origX - x); i++)
							sb.Append("LL");
						sb.Append("RDIL");
						for (int i = 0; i < Math.Abs(origX - x); i++)
							sb.Append("RR");
						fullMap[x][origY].value = 15;
						reviverSeeds++;
						return sb.ToString();
					}
					x--;
				}
				x = origX;
				while (x < FULL_WIDTH && fullMap[x][origY].value != 0x0F) {
					if (fullMap[x][origY].value == 8) {
						for (int i = 0; i < Math.Abs(origX - x); i++)
							sb.Append("RR");
						sb.Append("RDIL");
						for (int i = 0; i < Math.Abs(origX - x); i++)
							sb.Append("LL");
						fullMap[x][origY].value = 15;
						reviverSeeds++;
						return sb.ToString();
					}
					x++;
				}
			}
			else {
				while (y > 0 && fullMap[origX][y].value != 0x0F) {
					if (fullMap[origX][y].value == 8) {
						for (int i = 0; i < Math.Abs(origY - y); i++)
							sb.Append("UU");
						sb.Append("RDIL");
						for (int i = 0; i < Math.Abs(origY - y); i++)
							sb.Append("DD");
						fullMap[origX][y].value = 15;
						reviverSeeds++;
						return sb.ToString();
					}
					y--;
				}
				y = origY;
				while (y < FULL_HEIGHT && fullMap[origX][y].value != 0x0F) {
					if (fullMap[origX][y].value == 8) {
						for (int i = 0; i < Math.Abs(origY - y); i++)
							sb.Append("DD");
						sb.Append("RDIL");
						for (int i = 0; i < Math.Abs(origY - y); i++)
							sb.Append("UU");
						fullMap[origX][y].value = 15;
						reviverSeeds++;
						return sb.ToString();
					}
					y++;
				}
			}
			return "";
		}
	}
}
