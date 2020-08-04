#include "stdafx.h"
#include "JobExecutor.h"

JobExecutor::JobExecutor() {

}

void JobExecutor::StartThread(const size_t thread_count) {
	assert(thread_count >= 1);
	assert(thread_list_.empty());
	assert(go_on_ == false);

	go_on_ = true;
	thread_list_.resize(thread_count, nullptr);
	for (size_t i = 0; i < thread_count; ++i) {
		thread_list_[i] = new std::thread([this]() {
			this->Run();
		});
	}
}

void JobExecutor::StopThread() {
	go_on_ = false;
	for (size_t i = 0; i < thread_list_.size(); ++i) {
		conditional_variable_.setEvent();
	}

	for (std::thread* thread : thread_list_) {
		if (thread->joinable()) {
			thread->join();
		}
	}

	for (std::thread* thread : thread_list_) {
		delete thread;
	}
	thread_list_.clear();
}

void JobExecutor::Add(JobInterfacePtr job) {
	LockGuard guard(lock_);

	job_queue_.push(job);

	conditional_variable_.setEvent();
}

void JobExecutor::Run() {
	while (go_on_) {
		conditional_variable_.wait();

		while (go_on_) {
			JobInterfacePtr job;
			{
				LockGuard guard(lock_);

				if (job_queue_.empty()) {
					break;;
				}
				job = job_queue_.front();
				job_queue_.pop();
			}
			job->DoJob();
		}
	}
}

