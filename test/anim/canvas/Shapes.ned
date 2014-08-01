module Shapes
{
    @class(ArrangeFigures);
    @display("bgb=1000,1000");

    @figure[r1](type=rectangle;coords=10,0,200,10);
    @figure[r2](type=rectangle;coords=10,0,200,10;fillColor=#0000ff);
    @figure[r3](type=rectangle;coords=10,0,200,10;lineColor=#ff0000);
    @figure[r4](type=rectangle;coords=10,0,200,10;lineWidth=3);
    @figure[r5](type=rectangle;coords=10,0,200,10;lineStyle=solid);
    @figure[r6](type=rectangle;coords=10,0,200,10;lineStyle=dotted);
    @figure[r7](type=rectangle;coords=10,0,200,10;lineStyle=dashed);
    @figure[r8](type=rectangle;coords=10,0,200,10;fillColor=#0000ff;lineColor=#ff0000;lineWidth=3;lineStyle=dashed);

    @figure[ov1](type=oval;coords=10,0,200,10);
    @figure[ov2](type=oval;coords=10,0,200,10;fillColor=#0000ff);
    @figure[ov3](type=oval;coords=10,0,200,10;lineColor=#ff0000);
    @figure[ov4](type=oval;coords=10,0,200,10;lineWidth=3);
    @figure[ov5](type=oval;coords=10,0,200,10;lineStyle=solid);
    @figure[ov6](type=oval;coords=10,0,200,10;lineStyle=dotted);
    @figure[ov7](type=oval;coords=10,0,200,10;lineStyle=dashed);
    @figure[ov8](type=oval;coords=10,0,200,10;fillColor=#0000ff;lineColor=#ff0000;lineWidth=3;lineStyle=dashed);

    @figure[pc1](type=pieslice;coords=10,0,50,10;startAngle=0;endAngle=250);
    @figure[pc2](type=pieslice;coords=10,0,50,10;startAngle=0;endAngle=250;fillColor=#0000ff);
    @figure[pc3](type=pieslice;coords=10,0,50,10;startAngle=0;endAngle=250;lineColor=#ff0000);
    @figure[pc4](type=pieslice;coords=10,0,50,10;startAngle=0;endAngle=250;lineWidth=3);
    @figure[pc5](type=pieslice;coords=10,0,50,10;startAngle=0;endAngle=250;lineStyle=solid);
    @figure[pc6](type=pieslice;coords=10,0,50,10;startAngle=0;endAngle=250;lineStyle=dotted);
    @figure[pc7](type=pieslice;coords=10,0,50,10;startAngle=0;endAngle=250;lineStyle=dashed);
    @figure[pc8](type=pieslice;coords=10,0,50,10;startAngle=0;endAngle=250;fillColor=#0000ff;lineColor=#ff0000;lineWidth=3;lineStyle=dashed);

    @figure[pgn00](type=polygon;coords=10,0,30,18,100,5,200,5,200,0);
    @figure[pgn01](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillColor=#808080);
    @figure[pgn02](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillColor=#808080;lineColor=#00ff00);
    @figure[pgn04](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillColor=#808080;lineStyle=solid);
    @figure[pgn05](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillColor=#808080;lineStyle=dashed);
    @figure[pgn06](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillColor=#808080;lineStyle=dotted);
    @figure[pgn09](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillColor=#808080;lineWidth=3;lineStyle=solid);
    @figure[pgn08](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillColor=#808080;lineWidth=3;lineStyle=dashed);
    @figure[pgn10](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;fillColor=#808080;lineWidth=3;lineStyle=dotted);

    @figure[pgn20](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true);
    @figure[pgn21](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillColor=#808080);
    @figure[pgn22](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillColor=#808080;lineColor=#00ff00);
    @figure[pgn24](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillColor=#808080;lineStyle=solid);
    @figure[pgn25](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillColor=#808080;lineStyle=dashed);
    @figure[pgn26](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillColor=#808080;lineStyle=dotted);
    @figure[pgn29](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillColor=#808080;lineWidth=3;lineStyle=solid);
    @figure[pgn28](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillColor=#808080;lineWidth=3;lineStyle=dashed);
    @figure[pgn30](type=polygon;coords=10,0,30,18,100,5,200,5,200,0;smooth=true;fillColor=#808080;lineWidth=3;lineStyle=dotted);
}

