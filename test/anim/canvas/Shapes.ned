module Shapes
{
    @class(ArrangeFigures);
    @display("bgb=1000,1000");

    @figure[r1](type=rectangle;coords=10,0,200,10);
    @figure[r2](type=rectangle;coords=10,0,200,10;fillcolor=#0000ff);
    @figure[r3](type=rectangle;coords=10,0,200,10;linecolor=#ff0000);
    @figure[r4](type=rectangle;coords=10,0,200,10;linewidth=3);
    @figure[r5](type=rectangle;coords=10,0,200,10;linestyle=solid);
    @figure[r6](type=rectangle;coords=10,0,200,10;linestyle=dotted);
    @figure[r7](type=rectangle;coords=10,0,200,10;linestyle=dashed);
    @figure[r8](type=rectangle;coords=10,0,200,10;fillcolor=#0000ff;linecolor=#ff0000;linewidth=3;linestyle=dashed);

    @figure[ov1](type=oval;coords=10,0,200,10);
    @figure[ov2](type=oval;coords=10,0,200,10;fillcolor=#0000ff);
    @figure[ov3](type=oval;coords=10,0,200,10;linecolor=#ff0000);
    @figure[ov4](type=oval;coords=10,0,200,10;linewidth=3);
    @figure[ov5](type=oval;coords=10,0,200,10;linestyle=solid);
    @figure[ov6](type=oval;coords=10,0,200,10;linestyle=dotted);
    @figure[ov7](type=oval;coords=10,0,200,10;linestyle=dashed);
    @figure[ov8](type=oval;coords=10,0,200,10;fillcolor=#0000ff;linecolor=#ff0000;linewidth=3;linestyle=dashed);

    @figure[pgn00](type=polygon;coords=10,0,30,18,100,5,200,5,200,0);
    @figure[pgn01](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillcolor=#808080);
    @figure[pgn02](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillcolor=#808080;linecolor=#00ff00);
    @figure[pgn04](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillcolor=#808080;linestyle=solid);
    @figure[pgn05](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillcolor=#808080;linestyle=dashed);
    @figure[pgn06](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillcolor=#808080;linestyle=dotted);
    @figure[pgn09](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillcolor=#808080;linewidth=3;linestyle=solid);
    @figure[pgn08](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillcolor=#808080;linewidth=3;linestyle=dashed);
    @figure[pgn10](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillcolor=#808080;linewidth=3;linestyle=dotted);

    @figure[pgn20](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true);
    @figure[pgn21](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillcolor=#808080);
    @figure[pgn22](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillcolor=#808080;linecolor=#00ff00);
    @figure[pgn24](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillcolor=#808080;linestyle=solid);
    @figure[pgn25](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillcolor=#808080;linestyle=dashed);
    @figure[pgn26](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillcolor=#808080;linestyle=dotted);
    @figure[pgn29](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillcolor=#808080;linewidth=3;linestyle=solid);
    @figure[pgn28](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillcolor=#808080;linewidth=3;linestyle=dashed);
    @figure[pgn30](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillcolor=#808080;linewidth=3;linestyle=dotted);
}

