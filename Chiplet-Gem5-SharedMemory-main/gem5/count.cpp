// 此程序用于统计共享储存的通信文件并输出 popnet 的输入文件
// 该程序的工作原理是 1 将所有共享存储的通信记录一次性读入程序中。 2
// 读入完成后根据通信记 录的目标 chiplet 编号对通信记录分类。 3 分类之后按 cycle
// 排序。 4 选择 chiplet0 的 cycle，将其 他 chiplet 的 cycle 向 chiplet0
// 对齐，即同步大家的 cycle。 4 将所有 chiplet 的 cycle 排序，将通信 记录输出成
// trace 文件”bench”。 5 将各自 chiplet 在 3 中分类的而形成的类别，分文件输出。
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
class CommunicationRecord;
class Chiplet {
public:
  void setTime(unsigned long long time) { this->chipletStartCycle = time; }
  Chiplet() : chipletStartCycle(0), chipletNumber(-1) {
    // do nothing
  }
  int chipletNumber;
  unsigned long long chipletStartCycle;
  std::vector<CommunicationRecord *>
      relatedRecord; // if the srcCore number is the ChipletNumber, the record will be put here //
};
class CommunicationRecord {
public:
  CommunicationRecord() : cycle(0), srcCore(0), targetCore(0) {}
  CommunicationRecord(unsigned long long _cycle, int _srcCore,
                      int _targetCore) {
    this->cycle = _cycle;
    this->srcCore = _srcCore;
    this->targetCore = _targetCore;
  }
  unsigned long long cycle;
  int srcCore;
  int targetCore;
};
/*
 * this Program is aimed to generate the popnet trace file from the
 * communication record
 */
std::string communicationBaseFileName = "communication";
int TotalChipletNumber = 0;
unsigned long long chipletNumberZeroStartCycle;
void sortTheChipletRecord(unsigned long long *cycle, int *sequency, int length);
// 输出bench0.0 bench0.1 等文件
void outputSenderFile(Chiplet *ChipletSet) {
  for (size_t j = 0; j < ::TotalChipletNumber; j++) {
    std::ofstream file;
    int chipletNumber = j;
    std::string baseName = "bench.0.";
    baseName += char(chipletNumber + '0');
    file.open(baseName.c_str(), std::ios::out);
    // look for the record whose srcCore is j
    std::vector<CommunicationRecord *> tmpRecordSet;
    for (size_t i = 0; i < ::TotalChipletNumber; i++) {
      for (size_t k = 0; k < ChipletSet[i].relatedRecord.size(); k++) {
        CommunicationRecord *tmp = ChipletSet[i].relatedRecord[k];
        if (tmp->srcCore == chipletNumber) {
          tmpRecordSet.push_back(tmp);
        }
      }
    }
    // sort the record
    CommunicationRecord *record2sort =
        new CommunicationRecord[tmpRecordSet.size()]();
    int *sequency = new int[tmpRecordSet.size()]();
    unsigned long long *cycle = new unsigned long long[tmpRecordSet.size()];
    for (size_t j = 0; j < tmpRecordSet.size(); j++) {
      record2sort[j] = *tmpRecordSet[j];
      sequency[j] = j;
      cycle[j] = tmpRecordSet[j]->cycle;
    }
    sortTheChipletRecord(cycle, sequency, tmpRecordSet.size());
    for (int i = 0; i < tmpRecordSet.size(); i++) {
      record2sort[i] = *tmpRecordSet[sequency[i]];
    }
    // write to the sender file
    for (int i = 0; i < tmpRecordSet.size(); i++) {
      unsigned long long cycle = (record2sort[i].cycle - chipletNumberZeroStartCycle) / 1000;
      int targetCore = record2sort[i].targetCore;
      file << cycle << " 0 " << j << " 0 " << targetCore << " 5" << std::endl;
    }
    file.close();
  }
}
// 输出bench文件
void outputTheFile(CommunicationRecord *Record, int length) {
  std::ofstream file;
  file.open("bench", std::ios::out);
  for (size_t i = 0; i < length; i++) {
    unsigned long long cycle =(Record[i].cycle - chipletNumberZeroStartCycle) / 1000;
    int srcCore = Record[i].srcCore;
    int targetCore = Record[i].targetCore;
    file << cycle << " 0 " << srcCore << " 0 " << targetCore << " 5"
         << std::endl;
  }
  file.close();
}
// 用于生成全部trace文件
void generatePopnetTraceFile(Chiplet *chipletSet) {
  int recordSize = 0;
  for (size_t i = 0; i < ::TotalChipletNumber; i++) {
    recordSize += chipletSet[i].relatedRecord.size();
  }
  // bool* recordComplete = new bool[::TotalChipletNumber]();
  int *ptr2ChipletRecord = new int[::TotalChipletNumber]();
  CommunicationRecord *TotalRecord = new CommunicationRecord[recordSize]();
  for (size_t i = 0; i < recordSize; i++) {
    bool init = false;
    CommunicationRecord tmpRecord = CommunicationRecord(0, 0, 0);
    int targetChipletNumber = -1;
    for (size_t j = 0; j < ::TotalChipletNumber; j++) {
      if (ptr2ChipletRecord[j] >= chipletSet[j].relatedRecord.size()) {
        continue;
      }
      if (!init) {
        tmpRecord = *chipletSet[j].relatedRecord[ptr2ChipletRecord[j]];
        init = true;
        targetChipletNumber = j;
      }
      // compare the time and decide the min
      if ((chipletSet[j].relatedRecord[ptr2ChipletRecord[j]])->cycle <
          tmpRecord.cycle) {
        tmpRecord = *chipletSet[j].relatedRecord[ptr2ChipletRecord[j]];
        targetChipletNumber = j;
      }
    }
    TotalRecord[i] = tmpRecord;
    ptr2ChipletRecord[targetChipletNumber]++;
  }
  outputTheFile(TotalRecord, recordSize);
  outputSenderFile(&chipletSet[0]);
}
void swap(int *first, int *second) {
  int *tmp = first;
  second = tmp;
  first = second;
}
void swap(unsigned long long *first, unsigned long long *second) {
  unsigned long long *tmp = first;
  second = tmp;
  first = second;
}
void sortTheChipletRecord(unsigned long long *cycle, int *sequency,
                          int length) {
  // buble sort because not requring high performance
  for (int i = 0; i < length - 1; i++) {
    for (int j = 0; j < length - 1; j++) {
      if (cycle[j] > cycle[j + 1]) {
        swap(&cycle[j], &cycle[j + 1]);
        swap(&sequency[j], &sequency[j + 1]);
      }
    }
  }
}
// 排序时间，防止出现cycle无法对齐的情况
void sortChipletTime(Chiplet *chipletSet) {
  for (size_t i = 0; i < ::TotalChipletNumber; i++) {
    // for each chiplet
    Chiplet *currentChiplet = &chipletSet[i];
    int size = currentChiplet->relatedRecord.size();
    CommunicationRecord *tmp = new CommunicationRecord[size];
    int *sequency = new int[size]();
    unsigned long long *cycle = new unsigned long long[size];
    for (size_t j = 0; j < size; j++) {
      tmp[j] = *currentChiplet->relatedRecord[j];
      sequency[j] = j;
      cycle[j] = currentChiplet->relatedRecord[j]->cycle;
    }
    sortTheChipletRecord(cycle, sequency, (chipletSet[i]).relatedRecord.size());
    currentChiplet->relatedRecord.clear();
    for (int i = 0; i < size; i++) {
      currentChiplet->relatedRecord.push_back(&tmp[sequency[i]]);
    }
  }
}
// 查找chiplet的开始时间，因为gem5启动操作系统一般需要几十万cycle的开销。
void initChipletStartPoint(Chiplet *chiplet, bool isMainChiplet = false,
                           unsigned long long mainChipletCycle = 0) {
  // here is some stuff that something must be fixed in the future
  chiplet->setTime(chiplet->relatedRecord[0]->cycle);
  if (!isMainChiplet) {
    unsigned long long base = chiplet->relatedRecord[0]->cycle;
    chiplet->relatedRecord[0]->cycle = chiplet->relatedRecord[0]->cycle - base +
                                       mainChipletCycle + (rand() % 20) * 1000;
    for (size_t i = 1; i < chiplet->relatedRecord.size(); i++) {
      chiplet->relatedRecord[i]->cycle =
          chiplet->relatedRecord[i]->cycle - base + mainChipletCycle;
    }
  } else {
  }
}
void processOneFile(Chiplet *chipletSet, int currentChipletNumber) {
  std::ifstream myFile;
  std::string realFile =
      communicationBaseFileName + (char)(currentChipletNumber + '0');
  myFile.open(realFile.c_str(), std::ios::in);
  std::stringstream ss;
  std::string line = "";
  while (std::getline(myFile, line)) {
    ss.clear();
    ss.str(line);
    unsigned long long cycle;
    int srcCoreNumber;
    int targetCoreNumber;
    int data; // we don't need it but for skipping
    ss >> cycle >> srcCoreNumber >> targetCoreNumber >> data;
    if (targetCoreNumber == -1) { // the message is for all
	    CommunicationRecord *tmp = new CommunicationRecord(cycle, srcCoreNumber, currentChipletNumber);
for (size_t i = 0; i < ::TotalChipletNumber; i++) {
  chipletSet[i].relatedRecord.push_back(tmp);
}
    } else {
      chipletSet[targetCoreNumber].relatedRecord.push_back(
          new CommunicationRecord(cycle, srcCoreNumber, currentChipletNumber));
    }
  }
  myFile.close();
}
int main() {
  std::cout << "Enter the TotalChipletNumber" << std::endl;
  std::cin >> ::TotalChipletNumber;
  // bool* chipletInit = new bool[TotalChipletNumber]();
  Chiplet *myChipletSet = new Chiplet[::TotalChipletNumber]();
  for (int i = 0; i < TotalChipletNumber; i++) {
    myChipletSet[i].chipletNumber = i;
  }
  for (int i = 0; i < ::TotalChipletNumber; i++) {
    processOneFile(myChipletSet, i);
  }
  sortChipletTime(myChipletSet);
  initChipletStartPoint(&myChipletSet[0], true);
  for (size_t i = 1; i < ::TotalChipletNumber; i++) {
    initChipletStartPoint(&myChipletSet[i], false,
                          myChipletSet[0].chipletStartCycle);
  }
  chipletNumberZeroStartCycle = myChipletSet[0].relatedRecord[0]->cycle;
  generatePopnetTraceFile(myChipletSet);
  return 0;
}