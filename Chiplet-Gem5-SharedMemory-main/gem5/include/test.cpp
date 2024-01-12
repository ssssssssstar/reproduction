#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
 
using namespace std;
#include "m5ops.h"
/*
本示例示范 2 个 chiplet 做 10*10 的矩阵乘工作，假设结果为 C，则 C 的大小为
10*10，用一维矩阵储存，则 chiplet0 计算矩阵乘 C 索引从 0 到 500*249 的结果，而
chiplet1 计算剩下的结果。
*/
extern "C" 
{
	const int N = 10;
	int main() {
		// 程序初始化开始
		long long *martrix = (long long *)malloc(N * N * sizeof(long long));
		for (int i = 0; i < N * N; i++) {
			srand(i);
			martrix[i] = rand() % 10;
		}
		cout << "init martrix" << endl;
		
		long long *martrix2 = (long long *)malloc(N * N * sizeof(long long));
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				srand(i + j);
				martrix2[i * N + j] = rand() % 10;
			}
		}
		cout << "init martrix2" << endl;
		
		long long *martrix3 = (long long *)malloc(N * N * sizeof(long long));
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				for (int k = 0; k < N; k++)
					martrix3[i * N + j] = 0;
			}
		}
		cout << "init martrix3" << endl;
		
		// 初始化结束
		// 初始化 gem5 的相关信息
		int chipletNumber = -1;
		cout << "enter the ChipletNumber" << endl;
		std::cin >> chipletNumber;
		// 完成 gem5 的相关信息初始化
		m5_gadia_call(chipletNumber, chipletNumber, 0, 0); // 记录启始 cycle
	   // 示例的 totalChipletNumber 为 2，故不显式的写出来。
		if (chipletNumber == 0) {
			for (int i = N / 2; i < N; i++) {
				for (int j = 0; j < N; j++) {
					for (int k = 0; k < N; k++)
						martrix3[i * N + j] = martrix3[i * N + j] + martrix[i * N + k] * martrix2[k * N + j];
				}
			}
			m5_gadia_call(chipletNumber, 1, -2, 0);
			int position = 0;
			cout << " coming 0" << "coming while" << endl; 
			while (true) {
				int result = (int)m5_gadia_receive(chipletNumber);
				// 检测 chiplet1 是否完成了矩阵乘的工作
				if(result == -1)
					continue;
				else if (result == -2)  //代表等待的Chiplet已经完成读写 
					break;
				else {
					martrix3[position] = result;
					position++;
				}
			}
			m5_gadia_call(chipletNumber, chipletNumber, 0, 0); // 记录结束 cycle
			return 0;
		}// the following is responsible for collect
		else if (chipletNumber == 1) {
			for (int i = 0; i < N / 2; i++) {
				for (int j = 0; j < N; j++) {
					for (int k = 0; k < N; k++)
						martrix3[i * N + j] = martrix3[i * N + j] + martrix[i * N + k] * martrix2[k * N + j];
					// chiplet 1 把结果写入到共享储存中。
					m5_gadia_call(chipletNumber, 0, (int)martrix3[i * N + j], 0);
				}
			}
			cout << "coming 1" << endl;
			// 告诉 chiplet0, chiplet1 已经完成矩阵乘法
			m5_gadia_call(chipletNumber, 0, -2, 0); 
			return 0;
		}
	}
}