# TitleMaker
[Weighted Voronoi Stippling](https://www.cs.ubc.ca/labs/imager/tr/2002/secord2002b/secord.2002b.pdf)の実装  
画像を入力として、論文の手法で点の配置を行い、点に対してドロネー三角形分割を行った結果のcsvファイルを返す.  
CSVは三角形の各々のx,y座標と、その点の画像上でのrgbが記述されている  
例として,以下の画像を入力すると,  
![input_image](/sample_data/top.png)  
[delaunay_top_sample_30000_1.csv](/sample_data/delaunay_top_sample_30000_1.csv)のようなファイルをファイルを出力する.  
この3角形を画像に出力すると以下のようになる.  
![input_image](/sample_data/result.png)  

