// 0 for empty, 1 for player, 2 for box_unsuccess, 3 for wall,
// 4 for dest, 5 for box_success, 6 for player on the dest

int board1[8][20] = {
{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
{3,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,3},
{3,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,4,0,3},
{3,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,3},
{3,0,2,0,0,0,0,0,0,3,3,0,3,0,0,0,0,0,0,3},
{3,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,3},
{3,0,1,0,0,0,0,0,0,3,0,0,3,0,0,0,0,0,0,3},
{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3}
};

int board2[8][20] = {
0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
0,0,0,3,0, 0,0,0,0,0, 0,0,2,0,0, 0,0,0,0,0,
0,0,1,3,0, 0,3,3,3,3, 3,3,3,3,0, 0,0,0,0,0,
0,0,0,3,0, 0,3,0,0,0, 0,0,0,0,3, 0,0,0,0,0,
0,0,0,3,0, 0,4,0,0,3, 3,3,4,0,3, 0,0,0,0,0,
0,0,2,3,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,3,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,3,0,0,0, 0,0,0,0,0, 0,0,0,0,0
};

int board3[8][20] = {
3,3,0,0,3, 3,3,0,0,0, 3,0,0,0,0, 0,0,0,0,0,
0,0,0,2,0, 0,0,0,2,0, 3,0,0,0,0, 3,3,3,3,3,
0,0,0,3,0, 0,0,3,3,0, 3,0,0,0,0, 3,0,0,4,3,
0,0,0,3,0, 0,0,3,3,0, 0,0,0,0,0, 0,1,0,4,3,
0,0,0,3,0, 0,0,0,0,0, 0,0,3,0,0, 3,0,0,4,3,
0,3,0,0,0, 0,0,0,0,0, 0,0,3,0,0, 3,3,3,3,3,
0,0,3,0,2, 0,0,0,0,0, 0,0,3,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0, 0,0,3,0,0, 0,0,0,0,0
};