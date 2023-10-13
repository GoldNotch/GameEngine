#pragma once
#include <thread>
#include <msd/channel.hpp>

namespace Framework
{
	/// interface for outputing objects like renderer, sound outputer, etc
	template<typename TaskID, typename TaskT>
	struct IStreamProcessor
	{
		/// process_interval == 0 => no interval, fps non fixed, else fps fixed
		/// max_queue_size == 0 => no requirements to queue size, else fixed queue
		explicit IStreamProcessor(size_t queue_size = 0)
			: channel(queue_size) {};
		virtual ~IStreamProcessor() = default;

		void Run()
		{
			process_thread.reset(new std::thread(&IStreamProcessor::ProcessThreadMain, this));
		}

		void Shutdown()
		{
			channel.close();
			process_thread->join();
		}

		bool IsRunning() const noexcept
		{
			return process_thread != nullptr;
		}

		/// push data from other thread to this for processing
		void PushTask(TaskID task_id, TaskT && task_data)
		{
			channel << std::make_pair(task_id, std::forward(task_data)); // will wait if channel is full
		}

	protected:
		virtual void ProcessTask(TaskID task_id, const TaskT& task) = 0;

	private:
		msd::channel<std::pair<TaskID, TaskT>> channel; ///< channel to pass data in thread
		std::unique_ptr<std::thread> process_thread = nullptr; ///< thread for processing

	private:
		void ProcessThreadMain() noexcept
		{
			while (!channel.closed())
			{
				std::pair<TaskID, TaskT> task;
				channel >> task; // will wait until data is pushed into channel
				ProcessTask(task.first, task.second);
			}
		}
	};

}