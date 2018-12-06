/****************************************************************************************\
*                                     Circle Detection                                   *
\****************************************************************************************/
/*------------------------------------霍夫梯度法------------------------------------------*/
static void
icvHoughCirclesGradient( CvMat* img, float dp, float min_dist,
						int min_radius, int max_radius,
						int canny_threshold, int acc_threshold,
						CvSeq* circles, int circles_max )
{
	const int SHIFT = 10, ONE = 1 << SHIFT, R_THRESH = 30;  //One=1024,1左移10位2*10，R_THRESH是起始值，赋给max_count,后续会被覆盖。
	cv::Ptr<CvMat> dx, dy;  //Ptr是智能指针模板，将CvMat对象封装成指针
	cv::Ptr<CvMat> edges, accum, dist_buf;//edges边缘二值图像,accum为累加器图像,dist_buf存放候选圆心到满足条件的边缘点的半径
	std::vector<int> sort_buf;//用来进行排序的中间对象。在adata累加器排序中，其存放的是offset即偏移位置,int型。在ddata距离排序中，其存储的和下标是一样的值。
	cv::Ptr<CvMemStorage> storage;//内存存储器。创建的序列用来向其申请内存空间。
 
	int x, y, i, j, k, center_count, nz_count;//center_count为圆心数,nz_count为非零数
	float min_radius2 = (float)min_radius*min_radius;//最小半径的平方
	float max_radius2 = (float)max_radius*max_radius;//最大半径的平方
	int rows, cols, arows,acols;//rows,cols边缘图像的行数和列数,arows,acols是累加器图像的行数和列数
	int astep, *adata;//adata指向累加器数据域的首地址，用位置作为下标,astep为累加器每行的大小，以字节为单位
	float* ddata;//ddata即dist_data，距离数据
	CvSeq *nz, *centers;//nz为非0，即边界，centers为存放的候选中心的位置。
	float idp, dr;//idp即inv_dp,dp的倒数
	CvSeqReader reader;//顺序读取序列中的每个值
 
	edges = cvCreateMat( img->rows, img->cols, CV_8UC1 );//边缘图像
	cvCanny( img, edges, MAX(canny_threshold/2,1), canny_threshold, 3 );//调用canny，变为二值图像，0和非0即0和255
 
	dx = cvCreateMat( img->rows, img->cols, CV_16SC1 );//16位单通道图像，用来存储二值边缘图像的x方向的一阶导数
	dy = cvCreateMat( img->rows, img->cols, CV_16SC1 );//y方向的
	cvSobel( img, dx, 1, 0, 3 );//计算x方向的一阶导数
	cvSobel( img, dy, 0, 1, 3 );//计算y方向的一阶导数
 
	if( dp < 1.f )//控制dp不能比1小
		dp = 1.f;
	idp = 1.f/dp;
	accum = cvCreateMat( cvCeil(img->rows*idp)+2, cvCeil(img->cols*idp)+2, CV_32SC1 );//cvCeil返回不小于参数的最小整数。32为单通道
	cvZero(accum);//初始化累加器为0
 
	storage = cvCreateMemStorage();//创建内存存储器，使用默认参数0.默认大小为64KB
	nz = cvCreateSeq( CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), storage );//创建序列，用来存放非0点
	centers = cvCreateSeq( CV_32SC1, sizeof(CvSeq), sizeof(int), storage );//用来存放圆心
 
	rows = img->rows;
	cols = img->cols;
	arows = accum->rows - 2;
	acols = accum->cols - 2;
	adata = accum->data.i;//cvMat对象的union对象的i成员成员
	//step是矩阵中行的长度，单位为字节。我们使用到的矩阵是accum它的深度是CV_32SC1即32位的int 型。
	//如果我们知道一个指针如int* p;指向数组中的一个元素， 则可以通过p+accum->step/adata[0]来使指针移动到p指针所指元素的，正对的下一行元素
	astep = accum->step/sizeof(adata[0]);
 
	for( y = 0; y < rows; y++ )
	{
		const uchar* edges_row = edges->data.ptr + y*edges->step;   //边界存储的矩阵的每一行的指向行首的指针。
		const short* dx_row = (const short*)(dx->data.ptr + y*dx->step);//存储 x方向sobel一阶导数的矩阵的每一行的指向第一个元素的指针
		const short* dy_row = (const short*)(dy->data.ptr + y*dy->step);//y
 
		//遍历边缘的二值图像和偏导数的图像
		for( x = 0; x < cols; x++ )
		{
			float vx, vy;
			int sx, sy, x0, y0, x1, y1, r, k;
			CvPoint pt;
 
			vx = dx_row[x];//访问每一行的元素
			vy = dy_row[x];
 
			if( !edges_row[x] || (vx == 0 && vy == 0) )//如果在边缘图像（存储边缘的二值图像）某一点如A(x0,y0)==0则对一下点进行操作。vx和vy同时为0，则下一个
				continue;
 
			float mag = sqrt(vx*vx+vy*vy);//求梯度图像
			assert( mag >= 1 );//如果mag为0,说明没有边缘点，则stop。这里用了assert宏定义
			sx = cvRound((vx*idp)*ONE/mag);//  vx为该点的水平梯度（梯度幅值已经归一化）；ONE为为了用整数运算代替浮点数引入的一个因子，为2^10
			sy = cvRound((vy*idp)*ONE/mag);
 
			x0 = cvRound((x*idp)*ONE);
			y0 = cvRound((y*idp)*ONE);
 
			for( k = 0; k < 2; k++ )//k=0在梯度方向，k=1在梯度反方向对累加器累加。这里之所以要反向，因为对于一个圆上一个点，从这个点沿着斜率的方向的，最小半径到最大半径。在圆的另一边与其相对应的点，有对应的效果。
			{
				x1 = x0 + min_radius * sx; 
				y1 = y0 + min_radius * sy;
 
				for( r = min_radius; r <= max_radius; x1 += sx, y1 += sy, r++ )//x1=x1+sx即,x1=x0+min_radius*sx+sx=x0+(min_radius+1)*sx求得下一个点。sx为斜率
				{
					int x2 = x1 >> SHIFT, y2 = y1 >> SHIFT; //变回真实的坐标
					if( (unsigned)x2 >= (unsigned)acols ||   //如果x2大于累加器的行
						(unsigned)y2 >= (unsigned)arows )
						break;
					adata[y2*astep + x2]++;//由于c语言是按行存储的。即等价于对accum数组进行了操作。
				}
 
				sx = -sx; sy = -sy;
			}
 
			pt.x = x; pt.y = y;
			cvSeqPush( nz, &pt );//把非零边缘并且梯度不为0的点压入到堆栈
		}
	}
 
	nz_count = nz->total;
	if( !nz_count )//如果nz_count==0则返回
		return;
 
	for( y = 1; y < arows - 1; y++ )     //这里是从1到arows-1，因为如果是圆的话，那么圆的半径至少为1，即圆心至少在内层里面
	{
		for( x = 1; x < acols - 1; x++ )
		{
			int base = y*(acols+2) + x;//计算位置，在accum图像中
			if( adata[base] > acc_threshold &&
				adata[base] > adata[base-1] && adata[base] > adata[base+1] &&
				adata[base] > adata[base-acols-2] && adata[base] > adata[base+acols+2] )
				cvSeqPush(centers, &base);//候选中心点位置压入到堆栈。其候选中心点累加数大于阈值，其大于四个邻域
		}
	}
 
	center_count = centers->total;
	if( !center_count )    //如果没有符合条件的圆心，则返回到函数。
		return;
 
	sort_buf.resize( MAX(center_count,nz_count) );//重新分配容器的大小，取候选圆心的个数和非零边界的个数的最大值。因为后面两个均用到排序。
	cvCvtSeqToArray( centers, &sort_buf[0] );  //把序列转换成数组，即把序列centers中的数据放入到sort_buf的容器中。
 
	icvHoughSortDescent32s( &sort_buf[0], center_count, adata );//快速排序，根据sort_buf中的值作为下标，依照adata中对应的值进行排序,将累加值大的下标排到前面
	cvClearSeq( centers );//清空序列
	cvSeqPushMulti( centers, &sort_buf[0], center_count );//重新将中心的下标存入centers
 
 
	dist_buf = cvCreateMat( 1, nz_count, CV_32FC1 );//创建一个32为浮点型的一个行向量
	ddata = dist_buf->data.fl;//使ddata执行这个行向量的首地址
 
	dr = dp;
	min_dist = MAX( min_dist, dp );//如果输入的最小距离小于dp，则设在为dp
	min_dist *= min_dist;
 
 
	for( i = 0; i < centers->total; i++ )   //对于每一个中心点
	{
 
 
		int ofs = *(int*)cvGetSeqElem( centers, i );//获取排序的中心位置，adata值最大的元素，排在首位  ，offset偏移位置
		y = ofs/(acols+2) - 1;//这里因为edge图像比accum图像小两个边。
		x = ofs - (y+1)*(acols+2) - 1;//求得y坐标
		float cx = (float)(x*dp), cy = (float)(y*dp);
		float start_dist, dist_sum;
		float r_best = 0, c[3];
		int max_count = R_THRESH;
 
 
 
		for( j = 0; j < circles->total; j++ )//中存储已经找到的圆；若当前候选圆心与其中的一个圆心距离<min_dist，则舍弃该候选圆心
		{
			float* c = (float*)cvGetSeqElem( circles, j );//获取序列中的元素。
			if( (c[0] - cx)*(c[0] - cx) + (c[1] - cy)*(c[1] - cy) < min_dist )
				break;
		}
 
 
		if( j < circles->total )//当前候选圆心与任意已检测的圆心距离不小于min_dist时，才有j==circles->total
			continue;
	    cvStartReadSeq( nz, &reader );
		for( j = k = 0; j < nz_count; j++ )//每个候选圆心，对于所有的点
		{
			CvPoint pt;
			float _dx, _dy, _r2;
			CV_READ_SEQ_ELEM( pt, reader );
			_dx = cx - pt.x; _dy = cy - pt.y; //中心点到边界的距离
			_r2 = _dx*_dx + _dy*_dy;
			if(min_radius2 <= _r2 && _r2 <= max_radius2 )
			{
				ddata[k] = _r2; //把满足的半径的平方存起来
				sort_buf[k] = k;//sort_buf同上，但是这里的sort_buf的下标值和元素值是相同的,重新利用
				k++;//k和j是两个游标
			}
		}
 
		int nz_count1 = k, start_idx = nz_count1 - 1;
		if( nz_count1 == 0 )
			continue;  //如果一个候选中心到（非零边界且梯度>0）确定的点的距离中，没有满足条件的，则从下一个中心点开始。
		dist_buf->cols = nz_count1;//用来存放真是的满足条件的非零元素(三个约束：非零点，梯度不为0，到圆心的距离在min_radius和max_radius中间)
		cvPow( dist_buf, dist_buf, 0.5 );//对dist_buf中的元素开根号.求得半径
		icvHoughSortDescent32s( &sort_buf[0], nz_count1, (int*)ddata );////对与圆心的距离按降序排列，索引值在sort_buf中
 
 
		dist_sum = start_dist = ddata[sort_buf[nz_count1-1]];//dist距离，选取半径最小的作为起始值
 
 
		//下边for循环里面是一个算法。它定义了两个游标（指针）start_idx和j，j是外层循环的控制变量。而start_idx为控制当两个相邻的数组ddata的数据发生变化时，即d-start_dist>dr时，的步进。
		for( j = nz_count1 - 2; j >= 0; j-- )//从小到大。选出半径支持点数最多的半径
		{
			float d = ddata[sort_buf[j]];
 
			if( d > max_radius )//如果求得的候选圆点到边界的距离大于参数max_radius，则停止，因为d是第一个出现的最小的（按照从大到小的顺序排列的）
				break;
 
			if( d - start_dist > dr )//如果当前的距离减去最小的>dr(==dp)
			{
				float r_cur = ddata[sort_buf[(j + start_idx)/2]];//当前半径设为符合该半径的中值，j和start_idx相当于两个游标
				if( (start_idx - j)*r_best >= max_count*r_cur ||  //如果数目相等时，它会找半径较小的那个。这里是判断支持度的算法
					(r_best < FLT_EPSILON && start_idx - j >= max_count) ) //程序这个部分告诉我们，无法找到同心圆，它会被外层最大，支持度最多（支持的点最多）所覆盖。
				{
					r_best = r_cur;//如果 符合当前半径的点数(start_idx - j)/ 当前半径>= 符合之前最优半径的点数/之前的最优半径 || 还没有最优半径时,且点数>30时；其实直接把r_best初始值置为1即可省去第二个条件
					max_count = start_idx - j;//maxcount变为符合当前半径的点数，更新max_count值，后续的支持度大的半径将会覆盖前面的值。
				}
				start_dist = d;
				start_idx = j;
				dist_sum = 0;//如果距离改变较大，则重置distsum为0，再在下面的式子中置为当前值
			}
			dist_sum += d;//如果距离改变较小，则加上当前值（dist_sum）在这里好像没有用处。
		}
 
		if( max_count > R_THRESH )//符合条件的圆周点大于阈值30，则将圆心、半径压栈
		{
			c[0] = cx;
			c[1] = cy;
			c[2] = (float)r_best;
			cvSeqPush( circles, c );
			if( circles->total > circles_max )//circles_max是个很大的数,其值为INT_MAX
				return;
		}
	}
}
 
CV_IMPL CvSeq*
cvHoughCircles1( CvArr* src_image, void* circle_storage,
				int method, double dp, double min_dist,
				double param1, double param2,
				int min_radius, int max_radius )
{
	CvSeq* result = 0;
    CvMat stub, *img = (CvMat*)src_image;
	CvMat* mat = 0;
	CvSeq* circles = 0;
	CvSeq circles_header;
	CvSeqBlock circles_block;
	int circles_max = INT_MAX;
	int canny_threshold = cvRound(param1);//cvRound返回和参数最接近的整数值，对一个double类型进行四舍五入
	int acc_threshold = cvRound(param2);
 
	img = cvGetMat( img, &stub );//将img转化成为CvMat对象
 
	if( !CV_IS_MASK_ARR(img))  //图像必须为8位，单通道图像
		CV_Error( CV_StsBadArg, "The source image must be 8-bit, single-channel" );
 
	if( !circle_storage )
		CV_Error( CV_StsNullPtr, "NULL destination" );
 
	if( dp <= 0 || min_dist <= 0 || canny_threshold <= 0 || acc_threshold <= 0 )
		CV_Error( CV_StsOutOfRange, "dp, min_dist, canny_threshold and acc_threshold must be all positive numbers" );
 
	min_radius = MAX( min_radius, 0 );
	if( max_radius <= 0 )//用来控制当使用默认参数max_radius=0的时候
		max_radius = MAX( img->rows, img->cols );
	else if( max_radius <= min_radius )
		max_radius = min_radius + 2;
 
	if( CV_IS_STORAGE( circle_storage ))//如果传入的是内存存储器
	{
		circles = cvCreateSeq( CV_32FC3, sizeof(CvSeq),
			sizeof(float)*3, (CvMemStorage*)circle_storage );
 
	}
	else if( CV_IS_MAT( circle_storage ))//如果传入的参数时数组
	{
		mat = (CvMat*)circle_storage;
 
		//数组应该是CV_32FC3类型的单列数组。
		if( !CV_IS_MAT_CONT( mat->type ) || (mat->rows != 1 && mat->cols != 1) ||//连续,单列,CV_32FC3类型
			CV_MAT_TYPE(mat->type) != CV_32FC3 )
			CV_Error( CV_StsBadArg,
			"The destination matrix should be continuous and have a single row or a single column" );
		//将数组转换为序列
		circles = cvMakeSeqHeaderForArray( CV_32FC3, sizeof(CvSeq), sizeof(float)*3,
			mat->data.ptr, mat->rows + mat->cols - 1, &circles_header, &circles_block );//由于是单列，故elem_size为mat->rows+mat->cols-1
		circles_max = circles->total;
		cvClearSeq( circles );//清空序列的内容（如果传入的有数据的话）
	}
	else
		CV_Error( CV_StsBadArg, "Destination is not CvMemStorage* nor CvMat*" );
 
	switch( method )
	{
	case CV_HOUGH_GRADIENT:
		icvHoughCirclesGradient( img, (float)dp, (float)min_dist,
			min_radius, max_radius, canny_threshold,
			acc_threshold, circles, circles_max );
		break;
	default:
		CV_Error( CV_StsBadArg, "Unrecognized method id" );
	}
 
	if( mat )//给定一个指向圆存储的数组指针值，则返回0，即NULL
	{
		if( mat->cols > mat->rows )//因为不知道传入的是列向量还是行向量。
			mat->cols = circles->total;
		else
			mat->rows = circles->total;
	}
	else//如果是传入的是内存存储器，则返回一个指向一个序列的指针。
		result = circles;
 
	return result;
}
